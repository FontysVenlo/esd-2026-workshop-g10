# Fuzz Testing Workshop: Participant Guide

ESD semester 7 · AFL++ and Docker

## The idea in 30 seconds

You will use a fuzzer called AFL++ to attack a small program you have not seen. The fuzzer throws thousands of odd inputs at it every second. Your job is to be a detective: look at what the fuzzer found, work out **why** the program crashes, and prove it by hand.

Four words you will meet:

- **Docker container:** a ready-made mini computer on your laptop, with AFL++ already installed. You don't install anything yourself.
- **Seed:** a small, harmless starting input. AFL++ takes it and changes it again and again to make new inputs.
- **Crash:** the program stops with an error. Here the program calls `abort()` on purpose.
- **Minimize:** shrink a crashing input to the smallest version that still crashes the program.

## Before the workshop

1. Start Docker Desktop.
2. Clone the workshop repository.
3. From the repository root, open the workshop directory and build the image:

```bash
cd FuzzingWorkshop
docker compose build
```

The image is large and may take a while to download, so please do this before the session.

## Start the workshop container

On your computer, in the `FuzzingWorkshop/` folder, run:

```bash
docker compose run --rm workshop
```

Your terminal is now **inside the container**, in the folder `/workshop`. Every command below is typed there. First, set up AFL++ for Docker (copy and paste this once):

```bash
export AFL_SKIP_CPUFREQ=1 AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 AFL_NO_AFFINITY=1
```

These three settings tell AFL++ not to complain about things that Docker doesn't allow. You can ignore what they mean.

## Exercise 1: Find the crash (20 minutes)

Work in pairs. **Do not read `exercises/participant/participant.c` until step 7.** The point is to work out the bug from the clues, not from the code.

### Step 1: Compile the program

```bash
afl-cc exercises/participant/participant.c -o exercise
```

In plain words: this turns the C code into a program called `exercise`. AFL++'s own compiler (`afl-cc`) adds small markers inside it, so the fuzzer can see which parts of the code each input reaches.

### Step 2: Look at the seeds (clue 1)

```bash
ls exercises/participant/inputs1
wc -c exercises/participant/inputs1/*
cat exercises/participant/inputs1/seed1.txt
```

In plain words: the seeds are small, harmless starting inputs that are already in the repository. They do **not** crash the program. `wc -c` counts how many bytes (characters) each file has.

**Write down:** how many bytes do the seeds have?

### Step 3: Run the fuzzer

```bash
afl-fuzz -i exercises/participant/inputs1 -o /tmp/out2 -- ./exercise
```

In plain words: `-i` is the folder with your seeds, `-o` is where AFL++ saves its results, and `./exercise` is the program to attack.

You will see a status screen. Watch the line **saved crashes**. When it shows 1 or more, press `Ctrl+C` to stop.

**Look at the screen once before you stop it (clue 2):** compare **total crashes** with **total execs**. Is a crash rare or common? Would a very special input (like one exact secret word) crash that often?

### Step 4: Look at the crash file (clues 3 and 4)

```bash
ls /tmp/out2/default/crashes
wc -c /tmp/out2/default/crashes/id:*
od -c /tmp/out2/default/crashes/id:*
```

- `ls` shows the input that crashed the program.
- `wc -c` shows its size in bytes. **Compare it with the seed sizes from step 2.**
- `od -c` shows what is inside. **Do the characters look special, or random?**

**Write down your guess:** what do you think makes the program crash? Is it about *what* is in the input, or about something else?

### Step 5: Minimize the crash (clue 5)

```bash
f=$(find /tmp/out2/default/crashes -type f -name 'id:*' | head -n 1)
afl-tmin -i "$f" -o /tmp/min -- ./exercise
wc -c /tmp/min
```

In plain words: pick one crashing file, then let `afl-tmin` remove everything that isn't needed. What is the smallest size that still crashes? Does that fit your guess?

### Step 6: Test your guess by hand

Now prove it yourself. The command `printf 'text' | ./exercise` sends the text to the program, and `echo $?` prints how it ended:

- `0` means the program finished normally.
- `134` means the program crashed (aborted).

Try these:

```bash
printf '1234567' | ./exercise; echo $?
printf '12345678' | ./exercise; echo $?
```

Then try your own. Change the length and the characters and see what happens:

```bash
printf '1' | ./exercise; echo $?
printf 'abc' | ./exercise; echo $?
printf 'hello' | ./exercise; echo $?
printf '123456789' | ./exercise; echo $?
```

**Can you find the exact point where the program switches from `0` to `134`?** Does it matter which characters you use, or only how many?

### Step 7: Check your guess

Now open `exercises/participant/participant.c` and read it. Were you right?

## Where to look: summary of the clues

| Clue | Where to look | What it tells you |
|---|---|---|
| 1 | Seed sizes (`wc -c` in step 2) | What a harmless input looks like |
| 2 | Status screen: total crashes vs total execs | Whether crashing is rare or common |
| 3 | Crash file size (`wc -c` in step 4) | How the crash input differs from the seeds |
| 4 | Crash file content (`od -c` in step 4) | Whether the characters matter |
| 5 | Minimized file size (`wc -c` in step 5) | The smallest input that still crashes |
| 6 | Your own `printf` tests (step 6) | The exact boundary, confirmed by hand |

## Check yourself

- What is the rule that makes the program crash?
- Which clue helped you the most?
- How did AFL++ find the crash without reading the code?
- Why is the minimized input more useful to a developer than the first crash file?

## Troubleshooting

- **`no configuration file provided`:** run Docker Compose inside `FuzzingWorkshop/`.
- **Terminal too small:** make the window at least 79 by 24.
- **No free CPU core:** check that `AFL_NO_AFFINITY=1` is set.
- **Output directory already exists:** use a new folder, for example `/tmp/out3`, and use that name in the later commands too.
- **Seed crashes immediately:** use a shorter seed that doesn't crash.
- **Docker not running:** start Docker Desktop before using Compose.

## Why this matters

The crashing input files are your evidence. A good bug report contains the input that causes the crash and the steps to reproduce it. That is exactly what you have after step 6: a tiny input and one command that makes the program fail every time.
