# rootevent output
add_subdirectory(1_rootevent)

# no output
set(TESTING_ARGS --output=none)
simple_testing(io-none "--file=sm.gmad" "")

# store trajectories
simple_testing(io-store-trajectories "--file=1_storeTrajectories.gmad" "")

simple_testing(io-user-sampler           "--file=user_placed_sampler.gmad"             "")
simple_testing(io-user-sampler-rect      "--file=user_placed_sampler_rectangular.gmad" "")
simple_testing(io-user-sampler-ap        "--file=user_placed_sampler_ap_model.gmad"    "")
simple_fail(io-user-sampler-bad-shape "--file=user_placed_sampler_bad_shape.gmad"   "")


# checks - tests that should fail

# invalid sampler name - conflicts with protected names in output structures
simple_fail(io-invalid-sampler-name "--file=invalid_sampler_name.gmad")

# trying to open outfile file in invalid location
set(TESTING_ARGS --outfile=a/fictional/path/to/data)
simple_fail(io-invalid-file-path  "--file=sm.gmad")

# unknown output format
set(TESTING_ARGS --output=abcd)
simple_fail(io-unknown-output-format "--file=sm.gmad")