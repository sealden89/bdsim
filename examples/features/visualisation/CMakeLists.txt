add_subdirectory(3d)

simple_testing(custom-colour         "--file=coloured_rcol.gmad"       "")
simple_testing(custom-colour-alpha   "--file=coloured_rcol_alpha.gmad" "")
simple_testing(custom-colour-magnets "--file=coloured_sbend.gmad"      "")
