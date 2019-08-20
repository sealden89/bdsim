simple_fail(regions-invalid-region      "--file=invalid-regions.gmad"        "")

simple_testing(regions-region-defaults  "--file=regions.gmad"                "")
simple_testing(regions-global-default   "--file=regions-global-default.gmad" "")