simple_testing(field-sub-field-b           "--file=sub-field-b.gmad"            "")
simple_testing(field-sub-field-e           "--file=sub-field-e.gmad"            "")
simple_testing(field-sub-field-b-w-offsets "--file=sub-field-b-w-offsets.gmad"  "")
simple_testing(field-sub-field-e-w-offsets "--file=sub-field-e-w-offsets.gmad"  "")

simple_fail(field-sub-field-b-too-big       "--file=sub-field-b-too-big.gmad")
simple_fail(field-sub-field-e-too-big       "--file=sub-field-e-too-big.gmad")
