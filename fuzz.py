#!/usr/bin/env python3

import random
import subprocess
import sys
import time

valid_commands = [
    "echo 'fuzz test'",
    "echo $USER",
    'echo "quoted string"',
    "echo $((1+2))",
    "ls -l",
    "ls -a",
    "ls -lah",
    "pwd",
    "cd /tmp",
    "cd ..",
    "cd ~",
    "cd /",
    "touch fuzz.txt",
    "rm fuzz.txt",
    "rm -f fuzz.txt",
    "rm -rf fuzzdir",
    "mkdir fuzzdir",
    "mkdir -p fuzzdir/nested",
    "cp fuzz.txt fuzz_copy.txt",
    "cp -r fuzzdir fuzzdir_copy",
    "mv fuzz.txt moved.txt",
    "cat fuzz.txt",
    "head -n 5 fuzz.txt",
    "tail -n 3 fuzz.txt",
    "wc -l fuzz.txt",
    "grep 'test' fuzz.txt",
    "grep -E '[a-z]+' fuzz.txt",
    "find . -name '*.txt'",
    "find /tmp -type f",
    "chmod 644 fuzz.txt",
    "chmod +x script.sh",
    "chown $USER fuzz.txt",
    "true",
    "false",
    "exit",
    "return",
    "logout",
    "date",
    "whoami",
    "hostname",
    "uptime",
    "id",
    "uname -a",
    "env",
    "export FOO=bar",
    "unset FOO",
    "printenv",
    "set",
    "alias ll='ls -l'",
    "unalias ll",
    "sleep 1",
    "wait",
    "kill $$",
    "kill -9 99999",
    "xargs echo < fuzz.txt",
    "diff file1.txt file2.txt",
    "cmp file1.txt file2.txt",
    "sort fuzz.txt",
    "uniq fuzz.txt",
    "cut -d':' -f1 /etc/passwd",
    "awk '{print $1}' fuzz.txt",
    "sed 's/foo/bar/g' fuzz.txt",
    "yes 'fuzzing!'",
    "tee output.txt",
    "split -l 10 fuzz.txt",
    "tr a-z A-Z < fuzz.txt",
    "basename /path/to/file",
    "dirname /path/to/file",
    "which ls",
    "type ls",
    "hash",
    "command -v bash",
    "source ~/.bashrc",
    ". ~/.bashrc",
    "if true; then echo ok; fi",
    "for i in 1 2 3; do echo $i; done",
    "while false; do echo loop; done",
    "ssh localhost",
    "scp fuzz.txt localhost:/tmp/",
    "rsync -a fuzzdir/ /tmp/fuzzdir/",
    "wget http://example.com",
    "curl http://example.com",
    "curl -I http://example.com",
    "ping -c 1 8.8.8.8",
    "dig google.com",
    "host github.com",
    "nslookup openai.com",
    "df -h",
    "du -sh fuzzdir",
    "mount",
    "umount /mnt",
    "lsblk",
    "fdisk -l",
    "top -n 1",
    "htop",
    "ps aux",
    "jobs",
    "bg",
    "fg",
    "disown",
    "nice -n 10 sleep 1",
    "renice 10 $$",
    "tar -czf fuzz.tar.gz fuzzdir",
    "tar -xzf fuzz.tar.gz",
    "zip fuzz.zip fuzz.txt",
    "unzip fuzz.zip",
    "gzip fuzz.txt",
    "gunzip fuzz.txt.gz",
    "bzip2 fuzz.txt",
    "bunzip2 fuzz.txt.bz2",
    "crontab -l",
    "at now + 1 minute",
    "uptime",
    "dmesg | tail",
    "vmstat",
    "iostat",
    "echo 'this is $((2+3))'",
    'echo "escaped \\"quotes\\""',
    "test -f fuzz.txt && echo exists",
    "[ -d fuzzdir ] && echo dir",
    "[ 1 -eq 1 ] && echo yes",
    "printf 'val: %d\\n' 42",
    "let a=5+2; echo $a",
    ": > emptyfile",
    "> emptyfile",
    "< fuzz.txt cat",
    "command ls",
    "builtin echo 'builtin'",
]

valid_commands = [
    "echo 'fuzz test'",
    "ls -l",
    "ls -a",
    "pwd",
    "cd /tmp",
    "env",
    "export VAR=test",
    "echo $VAR",
    "touch fuzz.txt",
    "cat fuzz.txt",
    "mkdir -p fuzzdir",
    "rm -rf fuzzdir",
    "cp fuzz.txt fuzz_copy.txt",
    "mv fuzz.txt moved.txt",
    "grep 'fuzz' fuzz.txt",
    "find . -name fuzz.txt",
    "chmod 644 fuzz.txt",
    "ps aux",
    "kill -0 $$",
    "true",
    "false",
    "exit",
    "date",
    "whoami",
]

redirections = ["> fuzz.out", ">> fuzz.out", "< fuzz.txt"]
pipes = ["|"]
bad_tokens = [">>>", "<<", "| |", "-l cat", ">", "< >", ";;"]


def maybe_add_pipe_or_redir(cmds: list[str]) -> None:
    if len(cmds) == 0:
        return
    if random.random() < 0.3:
        cmds.append(random.choice(pipes + redirections))


def generate_validish_chain(max_len=4) -> str:
    count = random.randint(1, max_len)
    cmds = []
    for i in range(count):
        cmds.append(random.choice(valid_commands))
        maybe_add_pipe_or_redir(cmds)
    return " ".join(cmds)


def generate_invalid_chain(max_len=4) -> str:
    count = random.randint(1, max_len)
    cmds = []
    for i in range(count):
        if random.random() < 0.4:
            cmds.append(random.choice(bad_tokens))
        else:
            cmds.append(random.choice(valid_commands))
        maybe_add_pipe_or_redir(cmds)
    return " ".join(cmds)


def fuzz_shell(minishell_path="./minishell", n=1, invalid=False, delay=0.5):
    for _ in range(n):
        cmd = generate_invalid_chain() if invalid else generate_validish_chain()
        print(f"\n[INPUT] {cmd}")
        proc = subprocess.Popen(
            [minishell_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        try:
            out, err = proc.communicate(cmd + "\n", timeout=5)
            print("[OUTPUT]")
            print(out.strip())
            if err.strip():
                print("[STDERR]")
                print(err.strip())
        except subprocess.TimeoutExpired:
            proc.kill()
            print("[ERROR] Timeout")
        time.sleep(delay)


if __name__ == "__main__":
    count = 1
    invalid = False
    if "-n" in sys.argv:
        idx = sys.argv.index("-n")
        if idx + 1 < len(sys.argv):
            count = int(sys.argv[idx + 1])
    if "-i" in sys.argv:
        invalid = True
    fuzz_shell(n=count, invalid=invalid)
