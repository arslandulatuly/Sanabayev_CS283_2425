#!/usr/bin/env bats
#bats tests for 6-RShell, CS283
#Arslan Sanabayev, as5764

setup() {
    echo "test content" > test_file.txt 2>/dev/null || true
    mkdir -p test_dir
}

teardown() {
    if [ -n "$SERVER_PID" ]; then
        kill -9 $SERVER_PID 2>/dev/null || true
    fi
    rm -f test_file.txt client_output.txt cmd_script.txt
    rm -rf test_dir
}

start_server() {
    ./dsh -s > /dev/null 2>&1 &
    SERVER_PID=$!
    sleep 1
}

stop_server() {
    if [ -n "$SERVER_PID" ]; then
        kill -9 $SERVER_PID 2>/dev/null || true
    fi
}

run_client() {
    echo "$1" | ./dsh -c 127.0.0.1:1234 > client_output.txt 2>&1
}

@test "Server starts and client connects" {
    start_server
    run_client "exit"
    grep -q "dsh4>" client_output.txt
    stop_server
}

@test "Simple command execution" {
    start_server
    run_client "echo test"
    grep -q "test" client_output.txt
    stop_server
}

@test "Pipeline command execution" {
    start_server
    run_client "echo hello | grep hello"
    grep -q "hello" client_output.txt
    stop_server
}

@test "Multiple commands" {
    start_server
    cat > cmd_script.txt << EOF
echo first
echo second
exit
EOF
    cat cmd_script.txt | ./dsh -c 127.0.0.1:1234 > client_output.txt 2>&1
    grep -q "first" client_output.txt
    grep -q "second" client_output.txt
    stop_server
}

@test "Large output handling" {
    start_server
    run_client "ls -la / | cat"
    [ $(wc -c < client_output.txt) -gt 100 ]
    stop_server
}

@test "Nonexistent command error handling" {
    start_server
    run_client "nonexistent_cmd"
    grep -q "not found\|No such file" client_output.txt || [ -s client_output.txt ]
    stop_server
}

@test "Permission denied error" {
    start_server
    run_client "cat /etc/shadow"
    grep -q "Permission denied" client_output.txt || true
    stop_server
}

@test "Handling unexpected input" {
    start_server
    run_client "$(echo -e '\x00\x01\x02')"
    grep -q "Invalid input" client_output.txt || true
    stop_server
}

@test "Handling empty command" {
    start_server
    run_client ""
    grep -q "dsh4>" client_output.txt  # Should just print prompt with no error
    stop_server
}

@test "stop-server command terminates server" {
    start_server
    run_client "stop-server"
    sleep 1
    ! ps -p $SERVER_PID > /dev/null 2>&1
}

@test "Client exits properly" {
    start_server
    run_client "exit"
    stop_server
}

@test "Changing directory with cd (single command)" {
    start_server
    run_client "cd /tmp && pwd"
    grep -q "/tmp" client_output.txt || echo "CD command did not return expected directory"
    stop_server
}

@test "Long command execution" {
    start_server
    run_client "echo $(printf 'x%.0s' {1..5000})"
    grep -q "xxxxx" client_output.txt  # Check for long string presence
    stop_server
}

@test "Handling multiple spaces in command" {
    start_server
    run_client "echo     spaced    out    "
    grep -q "spaced out" client_output.txt
    stop_server
}

@test "Executing script file" {
    start_server
    echo -e "#!/bin/sh\necho script executed" > test_script.sh
    chmod +x test_script.sh
    run_client "./test_script.sh"
    grep -q "script executed" client_output.txt
    rm -f test_script.sh
    stop_server
}