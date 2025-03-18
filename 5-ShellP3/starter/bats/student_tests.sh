#!/usr/bin/env bats
# File: student_tests.sh
#test suite for Dragon Shell (dsh)
#Arslan Sanabayev, as5764, 03.16.2025, CS283

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF
ls
EOF
    # Assertions
    [ "$status" -eq 0 ]
}

@test "Built-in: exit command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "exiting..." ]]
}

@test "Built-in: rc command displays return code" {
    run ./dsh <<EOF
false
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "1" ]]
}

@test "Built-in: cd to valid directory" {
    run ./dsh <<EOF
cd /tmp
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/tmp" ]]
}

@test "Built-in: cd to invalid directory" {
    run ./dsh <<EOF
cd /nonexistent_directory
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "No such file or directory" ]]
    [[ "$output" =~ "2" ]] # ENOENT is typically 2
}

@test "Built-in: dragon command shows ASCII art" {
    run ./dsh <<EOF
dragon
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "%%%%%%%%" ]]
}

@test "Execution: simple command with arguments" {
    run ./dsh <<EOF
echo hello world
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello world" ]]
}

@test "Execution: command not found" {
    run ./dsh <<EOF
nonexistentcommand
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "No such file or directory" ]]
    [[ "$output" =~ "2" ]] # ENOENT
}

@test "Redirection: output redirection" {
    # Setup
    TEST_FILE="/tmp/dsh_test_output"
    rm -f "$TEST_FILE"

    run ./dsh <<EOF
echo "redirection test" > $TEST_FILE
EOF

    # Verify
    [ "$status" -eq 0 ]
    [ -f "$TEST_FILE" ]
    run cat "$TEST_FILE"
    [[ "$output" == "redirection test" ]]

    # Cleanup
    rm -f "$TEST_FILE"
}

@test "Redirection: append output" {
    # Setup
    TEST_FILE="/tmp/dsh_test_append"
    echo "line 1" > "$TEST_FILE"

    run ./dsh <<EOF
echo "line 2" >> $TEST_FILE
EOF

    # Verify
    [ "$status" -eq 0 ]
    run cat "$TEST_FILE"
    [[ "$output" == $'line 1\nline 2' ]]

    # Cleanup
    rm -f "$TEST_FILE"
}

@test "Redirection: input redirection" {
    # Setup
    TEST_FILE="/tmp/dsh_test_input"
    echo "test content" > "$TEST_FILE"

    run ./dsh <<EOF
cat < $TEST_FILE
EOF

    # Verify
    [ "$status" -eq 0 ]
    [[ "$output" =~ "test content" ]]

    # Cleanup
    rm -f "$TEST_FILE"
}

@test "Pipeline: simple two-command pipeline" {
    run ./dsh <<EOF
echo "hello world" | grep hello
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello world" ]]
}

@test "Pipeline: three-command pipeline" {
    run ./dsh <<EOF
echo "line 1\nline 2\nline 3" | grep line | wc -l
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "3" ]]
}

@test "Pipeline: return code reflects last command" {
    run ./dsh <<EOF
echo "test" | grep nonexistent
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "1" ]] # grep returns 1 when no matches found
}

@test "Error: built-in command in pipeline" {
    run ./dsh <<EOF
echo test | cd /tmp
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Built-in commands cannot be used in pipelines" ]]
}

@test "Quoting: argument with spaces" {
    run ./dsh <<EOF
echo "hello   world"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello   world" ]]
}

@test "Mixed: redirection and pipeline" {
    TEST_FILE="/tmp/dsh_test_mixed"
    rm -f "$TEST_FILE"

    echo -e "one\ntwo\nthree" | grep t > "$TEST_FILE.expected"

    run ./dsh <<EOF
echo -e "one\ntwo\nthree" | grep t > $TEST_FILE
EOF

    [ "$status" -eq 0 ]
    run diff "$TEST_FILE.expected" "$TEST_FILE"
    [ "$status" -eq 0 ]

    rm -f "$TEST_FILE" "$TEST_FILE.expected"
}
