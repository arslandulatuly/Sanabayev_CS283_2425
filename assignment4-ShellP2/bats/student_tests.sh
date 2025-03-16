#!/usr/bin/env bats

# File: student_tests.sh
# by Arslan Sanabayev, 03.15.2025
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]

	 
}

@test "execute pwd command" {
    run ./dsh <<EOF
pwd
EOF
    [ "$status" -eq 0 ]
}

# built-in command tests
@test "cd without arguments goes to home" {
    home_dir=$(eval echo ~)
    run ./dsh <<EOF
cd
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "$home_dir" ]]
}

@test "cd to non-existent directory gives error" {
    run ./dsh <<EOF
cd /fake_directory
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "No such file or directory" ]]
}

@test "cd to parent directory" {
    current_dir=$(pwd)
    parent_dir=$(dirname "$current_dir")

    run ./dsh <<EOF
cd ..
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "$parent_dir" ]]
}

# quoted argument handling
@test "echo single quoted string" {
    run ./dsh <<EOF
echo "hello world"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello world" ]]
}

@test "echo multiple quoted strings" {
    run ./dsh <<EOF
echo "hello" "world"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello world" ]]
}

@test "preserve spaces inside quotes" {
    run ./dsh <<EOF
echo "  spaced    text  "
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "  spaced    text  " ]]
}

# return code handling
@test "rc returns 0 after a successful command" {
    run ./dsh <<EOF
ls
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "0" ]]
}

@test "rc shows error after failed command" {
    run ./dsh <<EOF
doesnotexist
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Command not found in PATH" ]]
    [[ "$output" =~ "2" ]]
}

@test "rc handles permission denied" {
    run ./dsh <<EOF
touch testfile
chmod 000 testfile
./testfile
rc
rm -f testfile
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Permission denied" ]]
    [[ "$output" =~ "13" ]]
}

# command line parsing
@test "handle empty input" {
    run ./dsh <<EOF

EOF
    [ "$status" -eq 0 ]
}

@test "handle excessive spaces" {
    run ./dsh <<EOF
echo     spaced     out
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "spaced out" ]]
}

@test "handle tab-separated arguments" {
    run ./dsh <<EOF
echo "word1"  "word2"	"word3"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "word1 word2 word3" ]]
}

# exit command handling
@test "exit command terminates the shell" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}
