# Copyright (c) 2020-2021 The Linux Foundation
#
# SPDX-License-Identifier: Apache-2.0

import os

from west import log
from west.util import west_topdir, WestNotFound

from zspdx.cmakecache import parseCMakeCacheFile
from zspdx.cmakefileapijson import parseReply
from zspdx.datatypes import DocumentConfig, Document, File, PackageConfig, Package, RelationshipDataElementType, RelationshipData, Relationship
from zspdx.getincludes import getCIncludes
import zspdx.spdxids

# WalkerConfig contains configuration data for the Walker.
class WalkerConfig:
    def __init__(self):
        super(WalkerConfig, self).__init__()

        # prefix for Document namespaces; should not end with "/"
        self.namespacePrefix = ""

        # location of build directory
        self.buildDir = ""

        # should also analyze for included header files?
        self.analyzeIncludes = False

        # should also add an SPDX document for the SDK?
        self.includeSDK = False

# Walker is the main analysis class: it walks through the CMake codemodel,
# build files, and corresponding source and SDK files, and gathers the
# information needed to build the SPDX data classes.
class Walker:
    # initialize with WalkerConfig
    def __init__(self, cfg):
        super(Walker, self).__init__()

        # configuration - WalkerConfig
        self.cfg = cfg

        # the various Documents that we will be building
        self.docBuild = None
        self.docZephyr = None
        self.docApp = None
        self.docSDK = None

        # dict of absolute file path => the Document that owns that file
        self.allFileLinks = {}

        # queue of pending source Files to create, process and assign
        self.pendingSources = []

        # queue of pending relationships to create, process and assign
        self.pendingRelationships = []

        # parsed CMake codemodel
        self.cm = None

        # parsed CMake cache dict, once we have the build path
        self.cmakeCache = {}

        # C compiler path from parsed CMake cache
        self.compilerPath = ""

        # SDK install path from parsed CMake cache
        self.sdkPath = ""

    # primary entry point
    def makeDocuments(self):
        # parse CMake cache file and get compiler path
        log.inf("parsing CMake Cache file")
        self.getCacheFile()

        # parse codemodel from Walker cfg's build dir
        log.inf("parsing CMake Codemodel files")
        self.cm = self.getCodemodel()
        if not self.cm:
            log.err("could not parse codemodel from CMake API reply; bailing")
            return False

        # set up Documents
        log.inf("setting up SPDX documents")
        retval = self.setupDocuments()
        if not retval:
            return False

        # walk through targets in codemodel to gather information
        log.inf("walking through targets")
        self.walkTargets()

        # walk through pending sources and create corresponding files
        log.inf("walking through pending sources files")
        self.walkPendingSources()

        # walk through pending relationship data and create relationships
        log.inf("walking through pending relationships")
        self.walkRelationships()

        return True

    # parse cache file and pull out relevant data
    def getCacheFile(self):
        cacheFilePath = os.path.join(self.cfg.buildDir, "CMakeCache.txt")
        self.cmakeCache = parseCMakeCacheFile(cacheFilePath)
        if self.cmakeCache:
            self.compilerPath = self.cmakeCache.get("CMAKE_C_COMPILER", "")
            self.sdkPath = self.cmakeCache.get("ZEPHYR_SDK_INSTALL_DIR", "")

    # determine path from build dir to CMake file-based API index file, then
    # parse it and return the Codemodel
    def getCodemodel(self):
        log.dbg("getting codemodel from CMake API reply files")

        # make sure the reply directory exists
        cmakeReplyDirPath = os.path.join(self.cfg.buildDir, ".cmake", "api", "v1", "reply")
        if not os.path.exists(cmakeReplyDirPath):
            log.err(f'cmake api reply directory {cmakeReplyDirPath} does not exist')
            log.err('was query directory created before cmake build ran?')
            return None
        if not os.path.isdir(cmakeReplyDirPath):
            log.err(f'cmake api reply directory {cmakeReplyDirPath} exists but is not a directory')
            return None

        # find file with "index" prefix; there should only be one
        indexFilePath = ""
        for f in os.listdir(cmakeReplyDirPath):
            if f.startswith("index"):
                indexFilePath = os.path.join(cmakeReplyDirPath, f)
                break
        if indexFilePath == "":
            # didn't find it
            log.err(f'cmake api reply index file not found in {cmakeReplyDirPath}')
            return None

        # parse it
        return parseReply(indexFilePath)

    # set up Documents before beginning
    def setupDocuments(self):
        log.dbg("setting up placeholder documents")

        # set up build document
        cfgBuild = DocumentConfig()
        cfgBuild.name = "build"
        cfgBuild.namespace = self.cfg.namespacePrefix + "/build"
        cfgBuild.docRefID = "DocumentRef-build"
        self.docBuild = Document(cfgBuild)

        # we'll create the build packages in walkTargets()

        # the DESCRIBES relationship for the build document will be
        # with the zephyr_final package
        rd = RelationshipData()
        rd.ownerType = RelationshipDataElementType.DOCUMENT
        rd.ownerDocument = self.docBuild
        rd.otherType = RelationshipDataElementType.TARGETNAME
        rd.otherTargetName = "zephyr_final"
        rd.rlnType = "DESCRIBES"

        # add it to pending relationships queue
        self.pendingRelationships.append(rd)

        # set up zephyr document
        cfgZephyr = DocumentConfig()
        cfgZephyr.name = "zephyr-sources"
        cfgZephyr.namespace = self.cfg.namespacePrefix + "/zephyr"
        cfgZephyr.docRefID = "DocumentRef-zephyr"
        self.docZephyr = Document(cfgZephyr)

        # also set up zephyr sources package
        cfgPackageZephyr = PackageConfig()
        cfgPackageZephyr.name = "zephyr-sources"
        cfgPackageZephyr.spdxID = "SPDXRef-zephyr-sources"
        # relativeBaseDir is Zephyr sources topdir
        try:
            cfgPackageZephyr.relativeBaseDir = west_topdir(self.cm.paths_source)
        except WestNotFound:
            log.err(f"cannot find west_topdir for CMake Codemodel sources path {self.cm.paths_source}; bailing")
            return False
        pkgZephyr = Package(cfgPackageZephyr, self.docZephyr)
        self.docZephyr.pkgs[pkgZephyr.cfg.spdxID] = pkgZephyr

        # create DESCRIBES relationship data
        rd = RelationshipData()
        rd.ownerType = RelationshipDataElementType.DOCUMENT
        rd.ownerDocument = self.docZephyr
        rd.otherType = RelationshipDataElementType.PACKAGEID
        rd.otherPackageID = cfgPackageZephyr.spdxID
        rd.rlnType = "DESCRIBES"

        # add it to pending relationships queue
        self.pendingRelationships.append(rd)

        # set up app document
        cfgApp = DocumentConfig()
        cfgApp.name = "app-sources"
        cfgApp.namespace = self.cfg.namespacePrefix + "/app"
        cfgApp.docRefID = "DocumentRef-app"
        self.docApp = Document(cfgApp)

        # also set up app sources package
        cfgPackageApp = PackageConfig()
        cfgPackageApp.name = "app-sources"
        cfgPackageApp.spdxID = "SPDXRef-app-sources"
        # relativeBaseDir is app sources dir
        cfgPackageApp.relativeBaseDir = self.cm.paths_source
        pkgApp = Package(cfgPackageApp, self.docApp)
        self.docApp.pkgs[pkgApp.cfg.spdxID] = pkgApp

        # create DESCRIBES relationship data
        rd = RelationshipData()
        rd.ownerType = RelationshipDataElementType.DOCUMENT
        rd.ownerDocument = self.docApp
        rd.otherType = RelationshipDataElementType.PACKAGEID
        rd.otherPackageID = cfgPackageApp.spdxID
        rd.rlnType = "DESCRIBES"

        # add it to pending relationships queue
        self.pendingRelationships.append(rd)

        if self.cfg.includeSDK:
            # set up SDK document
            cfgSDK = DocumentConfig()
            cfgSDK.name = "sdk"
            cfgSDK.namespace = self.cfg.namespacePrefix + "/sdk"
            cfgSDK.docRefID = "DocumentRef-sdk"
            self.docSDK = Document(cfgSDK)

            # also set up zephyr sdk package
            cfgPackageSDK = PackageConfig()
            cfgPackageSDK.name = "sdk"
            cfgPackageSDK.spdxID = "SPDXRef-sdk"
            # relativeBaseDir is SDK dir
            cfgPackageSDK.relativeBaseDir = self.sdkPath
            pkgSDK = Package(cfgPackageSDK, self.docSDK)
            self.docSDK.pkgs[pkgSDK.cfg.spdxID] = pkgSDK

            # create DESCRIBES relationship data
            rd = RelationshipData()
            rd.ownerType = RelationshipDataElementType.DOCUMENT
            rd.ownerDocument = self.docSDK
            rd.otherType = RelationshipDataElementType.PACKAGEID
            rd.otherPackageID = cfgPackageSDK.spdxID
            rd.rlnType = "DESCRIBES"

            # add it to pending relationships queue
            self.pendingRelationships.append(rd)

        return True

    # walk through targets and gather information
    def walkTargets(self):
        log.dbg("walking targets from codemodel")

        # assuming just one configuration; consider whether this is incorrect
        cfgTargets = self.cm.configurations[0].configTargets
        for cfgTarget in cfgTargets:
            # build the Package for this target
            pkg = self.initConfigTargetPackage(cfgTarget)

            # see whether this target has any build artifacts at all
            if len(cfgTarget.target.artifacts) > 0:
                # add its build file
                bf = self.addBuildFile(cfgTarget, pkg)

                # get its source files
                self.collectPendingSourceFiles(cfgTarget, pkg, bf)
            else:
                log.dbg(f"  - target {cfgTarget.name} has no build artifacts")

            # get its target dependencies
            self.collectTargetDependencies(cfgTargets, cfgTarget, pkg)

    # build a Package in the Build doc for the given ConfigTarget
    def initConfigTargetPackage(self, cfgTarget):
        log.dbg(f"  - initializing Package for target: {cfgTarget.name}")

        # create target Package's config
        cfg = PackageConfig()
        cfg.name = cfgTarget.name
        cfg.spdxID = "SPDXRef-" + zspdx.spdxids.convertToSPDXIDSafe(cfgTarget.name)
        cfg.relativeBaseDir = self.cm.paths_build

        # build Package
        pkg = Package(cfg, self.docBuild)

        # add Package to build Document
        self.docBuild.pkgs[cfg.spdxID] = pkg
        return pkg

    # create a target's build product File and add it to its Package
    # call with:
    #   1) ConfigTarget
    #   2) Package for that target
    # returns: File
    def addBuildFile(self, cfgTarget, pkg):
        # assumes only one artifact in each target
        artifactPath = os.path.join(pkg.cfg.relativeBaseDir, cfgTarget.target.artifacts[0])
        log.dbg(f"  - adding File {artifactPath}")
        log.dbg(f"    - relativeBaseDir: {pkg.cfg.relativeBaseDir}")
        log.dbg(f"    - artifacts[0]: {cfgTarget.target.artifacts[0]}")

        # create build File
        bf = File(self.docBuild, pkg)
        bf.abspath = artifactPath
        bf.relpath = cfgTarget.target.artifacts[0]
        # can use nameOnDisk b/c it is just the filename w/out directory paths
        bf.spdxID = zspdx.spdxids.getUniqueFileID(cfgTarget.target.nameOnDisk, self.docBuild.timesSeen)
        # don't fill hashes / licenses / rlns now, we'll do that after walking

        # add File to Package
        pkg.files[bf.spdxID] = bf

        # add file path link to Document and global links
        self.docBuild.fileLinks[bf.abspath] = bf
        self.allFileLinks[bf.abspath] = self.docBuild

        # also set this file as the target package's build product file
        pkg.targetBuildFile = bf

        return bf

    # collect a target's source files, add to pending sources queue, and
    # create pending relationship data entry
    # call with:
    #   1) ConfigTarget
    #   2) Package for that target
    #   3) build File for that target
    def collectPendingSourceFiles(self, cfgTarget, pkg, bf):
        log.dbg(f"  - collecting source files and adding to pending queue")

        targetIncludesSet = set()

        # walk through target's sources
        for src in cfgTarget.target.sources:
            log.dbg(f"    - add pending source file and relationship for {src.path}")
            # get absolute path if we don't have it
            srcAbspath = src.path
            if not os.path.isabs(src.path):
                srcAbspath = os.path.join(self.cm.paths_source, src.path)

            # check whether it even exists
            if not (os.path.exists(srcAbspath) and os.path.isfile(srcAbspath)):
                log.dbg(f"  - {srcAbspath} does not exist but is referenced in sources for target {pkg.cfg.name}; skipping")
                continue

            # add it to pending source files queue
            self.pendingSources.append(srcAbspath)

            # create relationship data
            rd = RelationshipData()
            rd.ownerType = RelationshipDataElementType.FILENAME
            rd.ownerFileAbspath = bf.abspath
            rd.otherType = RelationshipDataElementType.FILENAME
            rd.otherFileAbspath = srcAbspath
            rd.rlnType = "GENERATED_FROM"

            # add it to pending relationships queue
            self.pendingRelationships.append(rd)

            # collect this source file's includes
            if self.cfg.analyzeIncludes and self.compilerPath:
                includes = self.collectIncludes(cfgTarget, pkg, bf, src)
                for inc in includes:
                    targetIncludesSet.add(inc)

        # make relationships for the overall included files,
        # avoiding duplicates for multiple source files including
        # the same headers
        targetIncludesList = list(targetIncludesSet)
        targetIncludesList.sort()
        for inc in targetIncludesList:
            # add it to pending source files queue
            self.pendingSources.append(inc)

            # create relationship data
            rd = RelationshipData()
            rd.ownerType = RelationshipDataElementType.FILENAME
            rd.ownerFileAbspath = bf.abspath
            rd.otherType = RelationshipDataElementType.FILENAME
            rd.otherFileAbspath = inc
            rd.rlnType = "GENERATED_FROM"

            # add it to pending relationships queue
            self.pendingRelationships.append(rd)

    # collect the include files corresponding to this source file
    # call with:
    #   1) ConfigTarget
    #   2) Package for this target
    #   3) build File for this target
    #   4) TargetSource entry for this source file
    # returns: sorted list of include files for this source file
    def collectIncludes(self, cfgTarget, pkg, bf, src):
        # get the right compile group for this source file
        if len(cfgTarget.target.compileGroups) < (src.compileGroupIndex + 1):
            log.dbg(f"    - {cfgTarget.target.name} has compileGroupIndex {src.compileGroupIndex} but only {len(cfgTarget.target.compileGroups)} found; skipping included files search")
            return []
        cg = cfgTarget.target.compileGroups[src.compileGroupIndex]

        # currently only doing C includes
        if cg.language != "C":
            log.dbg(f"    - {cfgTarget.target.name} has compile group language {cg.language} but currently only searching includes for C files; skipping included files search")
            return []

        srcAbspath = src.path
        if src.path[0] != "/":
            srcAbspath = os.path.join(self.cm.paths_source, src.path)
        return getCIncludes(self.compilerPath, srcAbspath, cg)

    # collect relationships for dependencies of this target Package
    # call with:
    #   1) all ConfigTargets from CodeModel
    #   2) this particular ConfigTarget
    #   3) Package for this Target
    def collectTargetDependencies(self, cfgTargets, cfgTarget, pkg):
        log.dbg(f"  - collecting target dependencies for {pkg.cfg.name}")

        # walk through target's dependencies
        for dep in cfgTarget.target.dependencies:
            # extract dep name from its id
            depFragments = dep.id.split(":")
            depName = depFragments[0]
            log.dbg(f"    - adding pending relationship for {depName}")

            # create relationship data between dependency packages
            rd = RelationshipData()
            rd.ownerType = RelationshipDataElementType.TARGETNAME
            rd.ownerTargetName = pkg.cfg.name
            rd.otherType = RelationshipDataElementType.TARGETNAME
            rd.otherTargetName = depName
            rd.rlnType = "HAS_PREREQUISITE"

            # add it to pending relationships queue
            self.pendingRelationships.append(rd)

            # if this is a target with any build artifacts (e.g. non-UTILITY),
            # also create STATIC_LINK relationship for dependency build files,
            # together with this Package's own target build file
            if len(cfgTarget.target.artifacts) == 0:
                continue

            # find the filename for the dependency's build product, using the
            # codemodel (since we might not have created this dependency's
            # Package or File yet)
            depAbspath = ""
            for ct in cfgTargets:
                if ct.name == depName:
                    # skip utility targets
                    if len(ct.target.artifacts) == 0:
                        continue
                    # all targets use the same relativeBaseDir, so this works
                    # even though pkg is the owner package
                    depAbspath = os.path.join(pkg.cfg.relativeBaseDir, ct.target.artifacts[0])
                    break
            if depAbspath == "":
                continue

            # create relationship data between build files
            rd = RelationshipData()
            rd.ownerType = RelationshipDataElementType.FILENAME
            rd.ownerFileAbspath = pkg.targetBuildFile.abspath
            rd.otherType = RelationshipDataElementType.FILENAME
            rd.otherFileAbspath = depAbspath
            rd.rlnType = "STATIC_LINK"

            # add it to pending relationships queue
            self.pendingRelationships.append(rd)

    # walk through pending sources and create corresponding files,
    # assigning them to the appropriate Document and Package
    def walkPendingSources(self):
        log.dbg(f"walking pending sources")

        # only one package in each doc; get it
        pkgZephyr = list(self.docZephyr.pkgs.values())[0]
        pkgApp = list(self.docApp.pkgs.values())[0]
        if self.cfg.includeSDK:
            pkgSDK = list(self.docSDK.pkgs.values())[0]

        for srcAbspath in self.pendingSources:
            # check whether we've already seen it
            srcDoc = self.allFileLinks.get(srcAbspath, None)
            srcPkg = None
            if srcDoc:
                log.dbg(f"  - {srcAbspath}: already seen, assigned to {srcDoc.cfg.name}")
                continue

            # not yet assigned; figure out where it goes
            pkgBuild = self.findBuildPackage(srcAbspath)

            if pkgBuild:
                log.dbg(f"  - {srcAbspath}: assigning to build document, package {pkgBuild.cfg.name}")
                srcDoc = self.docBuild
                srcPkg = pkgBuild
            elif self.cfg.includeSDK and os.path.commonpath([srcAbspath, pkgSDK.cfg.relativeBaseDir]) == pkgSDK.cfg.relativeBaseDir:
                log.dbg(f"  - {srcAbspath}: assigning to sdk document")
                srcDoc = self.docSDK
                srcPkg = pkgSDK
            elif os.path.commonpath([srcAbspath, pkgApp.cfg.relativeBaseDir]) == pkgApp.cfg.relativeBaseDir:
                log.dbg(f"  - {srcAbspath}: assigning to app document")
                srcDoc = self.docApp
                srcPkg = pkgApp
            elif os.path.commonpath([srcAbspath, pkgZephyr.cfg.relativeBaseDir]) == pkgZephyr.cfg.relativeBaseDir:
                log.dbg(f"  - {srcAbspath}: assigning to zephyr document")
                srcDoc = self.docZephyr
                srcPkg = pkgZephyr
            else:
                log.dbg(f"  - {srcAbspath}: can't determine which document should own; skipping")
                continue

            # create File and assign it to the Package and Document
            sf = File(srcDoc, srcPkg)
            sf.abspath = srcAbspath
            sf.relpath = os.path.relpath(srcAbspath, srcPkg.cfg.relativeBaseDir)
            filenameOnly = os.path.split(srcAbspath)[1]
            sf.spdxID = zspdx.spdxids.getUniqueFileID(filenameOnly, srcDoc.timesSeen)
            # don't fill hashes / licenses / rlns now, we'll do that after walking

            # add File to Package
            srcPkg.files[sf.spdxID] = sf

            # add file path link to Document and global links
            srcDoc.fileLinks[sf.abspath] = sf
            self.allFileLinks[sf.abspath] = srcDoc

    # figure out which build Package contains the given file, if any
    # call with:
    #   1) absolute path for source filename being searched
    def findBuildPackage(self, srcAbspath):
        # Multiple target Packages might "contain" the file path, if they
        # are nested. If so, the one with the longest path would be the
        # most deeply-nested target directory, so that's the one which
        # should get the file path.
        pkgLongestMatch = None
        for pkg in self.docBuild.pkgs.values():
            if os.path.commonpath([srcAbspath, pkg.cfg.relativeBaseDir]) == pkg.cfg.relativeBaseDir:
                # the package does contain this file; is it the deepest?
                if pkgLongestMatch:
                    if len(pkg.cfg.relativeBaseDir) > len(pkgLongestMatch.cfg.relativeBaseDir):
                        pkgLongestMatch = pkg
                else:
                    # first package containing it, so assign it
                    pkgLongestMatch = pkg

        return pkgLongestMatch

    # walk through pending RelationshipData entries, create corresponding
    # Relationships, and assign them to the applicable Files / Packages
    def walkRelationships(self):
        for rlnData in self.pendingRelationships:
            rln = Relationship()
            # get left side of relationship data
            docA, spdxIDA, rlnsA = self.getRelationshipLeft(rlnData)
            if not docA or not spdxIDA:
                continue
            rln.refA = spdxIDA
            # get right side of relationship data
            spdxIDB = self.getRelationshipRight(rlnData, docA)
            if not spdxIDB:
                continue
            rln.refB = spdxIDB
            rln.rlnType = rlnData.rlnType
            rlnsA.append(rln)
            log.dbg(f"  - adding relationship to {docA.cfg.name}: {rln.refA} {rln.rlnType} {rln.refB}")

    # get owner (left side) document and SPDX ID of Relationship for given RelationshipData
    # returns: doc, spdxID, rlnsArray (for either Document, Package, or File, as applicable)
    def getRelationshipLeft(self, rlnData):
        if rlnData.ownerType == RelationshipDataElementType.FILENAME:
            # find the document for this file abspath, and then the specific file's ID
            ownerDoc = self.allFileLinks.get(rlnData.ownerFileAbspath, None)
            if not ownerDoc:
                log.dbg(f"  - searching for relationship, can't find document with file {rlnData.ownerFileAbspath}; skipping")
                return None, None, None
            sf = ownerDoc.fileLinks.get(rlnData.ownerFileAbspath, None)
            if not sf:
                log.dbg(f"  - searching for relationship for file {rlnData.ownerFileAbspath} points to document {ownerDoc.cfg.name} but file not found; skipping")
                return None, None, None
            # found it
            if not sf.spdxID:
                log.dbg(f"  - searching for relationship for file {rlnData.ownerFileAbspath} found file, but empty ID; skipping")
                return None, None, None
            return ownerDoc, sf.spdxID, sf.rlns
        elif rlnData.ownerType == RelationshipDataElementType.TARGETNAME:
            # find the document for this target name, and then the specific package's ID
            # for target names, must be docBuild
            ownerDoc = self.docBuild
            # walk through target Packages and check names
            for pkg in ownerDoc.pkgs.values():
                if pkg.cfg.name == rlnData.ownerTargetName:
                    if not pkg.cfg.spdxID:
                        log.dbg(f"  - searching for relationship for target {rlnData.ownerTargetName} found package, but empty ID; skipping")
                        return None, None, None
                    return ownerDoc, pkg.cfg.spdxID, pkg.rlns
            log.dbg(f"  - searching for relationship for target {rlnData.ownerTargetName}, target not found in build document; skipping")
            return None, None, None
        elif rlnData.ownerType == RelationshipDataElementType.DOCUMENT:
            # will always be SPDXRef-DOCUMENT
            return rlnData.ownerDocument, "SPDXRef-DOCUMENT", rlnData.ownerDocument.relationships
        else:
            log.dbg(f"  - unknown relationship type {rlnData.ownerType}; skipping")
            return None, None, None

    # get other (right side) SPDX ID of Relationship for given RelationshipData
    def getRelationshipRight(self, rlnData, docA):
        if rlnData.otherType == RelationshipDataElementType.FILENAME:
            # find the document for this file abspath, and then the specific file's ID
            otherDoc = self.allFileLinks.get(rlnData.otherFileAbspath, None)
            if not otherDoc:
                log.dbg(f"  - searching for relationship, can't find document with file {rlnData.otherFileAbspath}; skipping")
                return None
            bf = otherDoc.fileLinks.get(rlnData.otherFileAbspath, None)
            if not bf:
                log.dbg(f"  - searching for relationship for file {rlnData.otherFileAbspath} points to document {otherDoc.cfg.name} but file not found; skipping")
                return None
            # found it
            if not bf.spdxID:
                log.dbg(f"  - searching for relationship for file {rlnData.otherFileAbspath} found file, but empty ID; skipping")
                return None
            # figure out whether to append DocumentRef
            spdxIDB = bf.spdxID
            if otherDoc != docA:
                spdxIDB = otherDoc.cfg.docRefID + ":" + spdxIDB
                docA.externalDocuments.add(otherDoc)
            return spdxIDB
        elif rlnData.otherType == RelationshipDataElementType.TARGETNAME:
            # find the document for this target name, and then the specific package's ID
            # for target names, must be docBuild
            otherDoc = self.docBuild
            # walk through target Packages and check names
            for pkg in otherDoc.pkgs.values():
                if pkg.cfg.name == rlnData.otherTargetName:
                    if not pkg.cfg.spdxID:
                        log.dbg(f"  - searching for relationship for target {rlnData.otherTargetName} found package, but empty ID; skipping")
                        return None
                    spdxIDB = pkg.cfg.spdxID
                    if otherDoc != docA:
                        spdxIDB = otherDoc.cfg.docRefID + ":" + spdxIDB
                        docA.externalDocuments.add(otherDoc)
                    return spdxIDB
            log.dbg(f"  - searching for relationship for target {rlnData.otherTargetName}, target not found in build document; skipping")
            return None
        elif rlnData.otherType == RelationshipDataElementType.PACKAGEID:
            # will just be the package ID that was passed in
            return rlnData.otherPackageID
        else:
            log.dbg(f"  - unknown relationship type {rlnData.otherType}; skipping")
            return None
