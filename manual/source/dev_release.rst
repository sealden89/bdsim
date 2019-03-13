.. _dev-release:

Release Checklist
*****************

Things to update immediately before a release  (i.e. from a release candidate branch):

1. README - update at the top and the version history.
2. CMakeLists.txt - change major, minor and patch version at the very top.

For each submodule:
    * Update version in setup.py.
    * Update version in setup.cfg.
    * Update version in docs/source/conf.py in two places.
    * Update version history in submodule manual.
    * Generate submodule documentation (html + latexpdf); copy pdf to docs dir.
    * Update main __init__.py / __version__ number to match.
    * Commit pdf of documentation to each submodule.
    * Tag submodule version.
    * Upload submodule manual to website.
    * Upload to pypi.

3. Tag submodule repository versions.
4. Update submodules in bdsim repository.
5. If the data format has changed increment the data version in output
   header structure.
   
   * Check data version in analysis.
   * Check data version in :code:`analysis/DataLoader.cc` initialiser list.
   * Regenerate data samples in :code:`examples/features/data/`.
   * Regenerate data sample :code:`examples/features/beam/userfile-sample.root`.


6. Update version history (including submodule and data versions) in
   manual source.

   * Submodule versions.
   * Data version.
   * Data class versions.


7. If BDSColours has changed, run BDSIM with DEBUGOUTPUT build and copy print out of
   colours to manual (already in correct format).
8. Regenerate BDSIM manual and commit new version. Upload to website.
9. Merge release candidate branch back into develop.
10. In develop, put back README, CMakeLists.txt to new version.develop.
11. Merge release candidate branch into master then delete.
12. Check all tests complete locally given merge before pushing.
13. Tag master branch for version number.
