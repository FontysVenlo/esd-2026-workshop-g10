# Fuzz Testing Workshop: Structure and Script

Presenters: Anna Zhakhyan and Francesco Andrei. Duration: 90 minutes. ESD semester 7

## 1. Plan at a glance

The workshop runs 90 minutes over 17 slides, and about 36 minutes of it is hands-on: a manual exercise, a live AFL++ demo and an AFL++ exercise. Your deck has 10 usable slides today; the 7 marked NEW below are what's missing. Also delete pages 07 to 15 of the 'Fradel and Spies' template (memory vs storage, how to buy a computer and so on), fix 'Iceberaker', and fix the duplicate page numbers.

| # | Slide | Status | Speaker | Time |
| --- | --- | --- | --- | --- |
| 1 | Title and agenda (add times to the agenda) | existing | Francesco | 0:00 to 0:02 |
| 2 | How would you break this? (icebreaker) | existing | Francesco | 0:02 to 0:07 |
| 3 | Introduction to fuzz testing | existing | Anna | 0:07 to 0:11 |
| 4 | Where does fuzzing come from? | NEW | Anna | 0:11 to 0:14 |
| 5 | The fuzz testing process | existing | Anna | 0:14 to 0:17 |
| 6 | Types of fuzz testing | existing | Anna | 0:17 to 0:20 |
| 7 | How is fuzz testing different? | existing | Francesco | 0:20 to 0:24 |
| 8 | Fuzzing tools landscape | NEW | Francesco | 0:24 to 0:28 |
| 9 | How AFL++ works | NEW | Francesco | 0:28 to 0:32 |
| 10 | Exercise 1: become the fuzzer | existing | Both | 0:32 to 0:40 |
| 11 | Live demo with AFL++ | NEW | Francesco drives, Anna narrates | 0:40 to 0:48 |
| - | Break | - | - | 0:48 to 0:52 |
| 12 | Exercise 2: find the crash | NEW | Both | 0:52 to 1:12 |
| 13 | Why use fuzz testing? | existing | Francesco | 1:12 to 1:15 |
| 14 | Fuzz testing limitations | existing | Anna | 1:15 to 1:19 |
| 15 | Kahoot quiz | existing | Anna | 1:19 to 1:26 |
| 16 | Conclusions | NEW | Both | 1:26 to 1:29 |
| 17 | Sources and questions | NEW | Both | 1:29 to 1:30 |


## 2. Setup and commands for the demo and the exercise

Everything runs in your AFL++ container, and participants only need Docker. I'm assuming your two C files are called `exercises/demo/demo.c` (crashes on the first byte 'X') and `exercises/participant/participant.c` (crashes on 8 or more bytes); rename in the commands if yours differ.

**Before the workshop**

- The `aflplusplus/aflplusplus` image is large. Message everyone one or two days ahead: 'clone the repo, change into `FuzzingWorkshop`, and run `docker compose build` before the workshop'. Also build it on a laptop on the university Wi-Fi to see how long it takes.
- Add `in/` and `out/` to `.gitignore`. The commands below write to `/tmp` inside the container, which avoids both clutter and the slow bind mount on Windows and Mac.
- Run the whole demo once with a timer. AFL++ should find both crashes within seconds; note how long, so you know how long to leave it on screen.
- Record a short screen recording of the demo as a fallback if Docker fails on stage.

**Commands (put them on slides 11 and 12 and in the README)**

```
# on your laptop, from the repository root
cd FuzzingWorkshop
docker compose build
docker compose run --rm workshop

# inside the container (working directory /workshop)
export AFL_SKIP_CPUFREQ=1 AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 AFL_NO_AFFINITY=1

# 1. compile with AFL++ instrumentation
afl-cc exercises/demo/demo.c -o demo

# 2. a seed input (it must NOT crash the program)
mkdir -p /tmp/in && printf 'A' > /tmp/in/seed

# 3. fuzz (stop with Ctrl+C)
afl-fuzz -i /tmp/in -o /tmp/out -- ./demo

# 4. look at the saved crashes
ls /tmp/out/default/crashes
f=$(find /tmp/out/default/crashes -type f -name 'id:*' | head -n 1)
od -c "$f"

# 5. replay the crash by hand (exit code 134 = aborted)
./demo < "$f"; echo $?
```

For Exercise 2, use `exercises/participant/participant.c` and a second seed (`printf 'hi' > /tmp/in2/seed`, two bytes, so it doesn't crash) and a second output folder. To shrink a crash to its smallest form:

```
afl-cc exercises/participant/participant.c -o exercise
mkdir -p /tmp/in2 && printf 'hi' > /tmp/in2/seed
afl-fuzz -i /tmp/in2 -o /tmp/out2 -- ./exercise
f=$(find /tmp/out2/default/crashes -type f -name 'id:*' | head -n 1)
wc -c /tmp/out2/default/crashes/id:*
afl-tmin -i "$f" -o /tmp/min -- ./exercise
wc -c /tmp/min
```

**If AFL++ complains** (I couldn't run it here, so test these):

- 'core_pattern' or missing crashes message: the two `AFL_...` variables above should silence it.
- 'no free CPU core': add `export AFL_NO_AFFINITY=1`.
- 'output directory exists': use a fresh output folder such as `/tmp/out-new`, or delete only the old workshop run's output after confirming you no longer need it.
- For repeatable demonstrations, choose a fresh output directory each run.
- Run Docker Compose from `FuzzingWorkshop/`, not the repository root.
- If AFL++ reports the terminal is too small, enlarge it to at least 79 columns by 24 rows.
- 'program crashed with one of the test cases provided': your seed crashes the program; use a shorter seed for `exercise.c`.

## 3. Script, slide by slide (part 1: intro, 0:00 to 0:32)

Lines in **bold with a name** are spoken. Text in [brackets] is an action. Sources are named out loud so the graders hear the research.

### Slide 1: Title and agenda (Francesco, 2 min)

**Francesco:** Hi everyone, welcome to our workshop on fuzz testing. I'm Francesco and this is my colleague Anna

**Francesco:** In the next 90 minutes you'll learn what fuzz testing is, how it differs from the testing you already know, and then you'll do it yourselves: first by hand, then with a real fuzzer called AFL++ that runs in Docker.

**Anna:** [Point at the agenda.] We start with an icebreaker and an introduction, then a first exercise, a live demo, a short break, the main exercise, then advantages and limitations, a Kahoot quiz and conclusions. Ask questions whenever you like. Everything we show today is in our GitHub repository. The Docker instructions are in the FuzzingWorkshop folder.

**Francesco:** Quick check before we start: who already has the Docker image built? [Hands.] If you don't, open `FuzzingWorkshop` and run `docker compose build` now. It works in the background while we talk.

### Slide 2: How would you break this? (Francesco, 5 min)

**Francesco:** Let's start with a question. On the screen is a tiny app that asks for your age. Imagine your goal is to make it behave unexpectedly. You have 30 seconds: turn to your neighbour and decide what you would type in. Go.

[Wait 30 seconds, then ask three or four people.]

**Francesco:** Time. What did you try? [Expect letters, negative numbers, huge numbers, empty input, emoji, SQL.] Nobody typed their real age. You tried empty, negative, huge and the wrong type. That instinct is exactly what fuzz testing automates. Anna will explain how.

### Slide 3: Introduction to fuzz testing (Anna, 4 min)

**Anna:** Thanks Francesco. Fuzz testing, or fuzzing, is a technique where we feed an application random, invalid, unexpected or malformed input and watch what happens. The goal is to find out whether the software can handle unusual input safely, without crashing, failing or exposing security vulnerabilities. 

**Anna:** Why does that matter? Three reasons, which are the three boxes. First, it helps detect input validation problems, crashes and security weaknesses. Second, it's commonly used on applications, APIs, file parsers and network protocols, so anything that reads input from the outside. Third, itmakes software more robust and reliable by testing conditions nobody planned for.

**Anna:** Whenever your program reads a file, a network packet or a form field, someone else controls that input. Fuzzing asks: what happens when it's garbage?

### Slide 4: Where does fuzzing come from? (Anna, 3 min) NEW

**Slide content:** a three-step timeline. (1) 1988: noise on a dial-up line during a storm crashes Unix programs. (2) 1990: Miller's study crashes a large share of standard Unix tools with random input. (3) Today: coverage-guided fuzzers such as AFL++ and continuous fuzzing services such as OSS-Fuzz.

**Anna:** The idea is older than you might think. In 1988 Professor Barton Miller at the University of Wisconsin was logged in to a Unix machine over a dial-up line during a thunderstorm. Line noise corrupted what he typed, and the programs he was using crashed.

**Anna:** That made him curious. He and his students wrote a program that threw random characters at standard Unix utilities and published the results in 1990. Somewhere between a quarter and a third of the tools crashed or hung.  That's where the word 'fuzz' comes from.

**Anna:** So fuzzing started with a very dumb idea, random data, and it worked. Today's fuzzers are much smarter, and some, like Google's OSS-Fuzz, run continuously on open-source projects. You'll meet one of them in a few minutes.

### Slide 5: The fuzz testing process (Anna, 3 min)

**Anna:** Here's the process, in six steps. One: identify the target system. Two: identify its inputs, like forms, files, API parameters or network messages. Three: generate fuzzed data. Four: execute the test using that data. Five: monitor the system's behaviour, looking for crashes, hangs or strange responses. Six: log the defects, which means saving the exact input that caused the failure so it can be reproduced.

**Anna:** Steps three to five are what we automate. In the demo, the fuzzer does them thousands of times per second while we watch.

### Slide 6: Types of fuzz testing (Anna, 3 min)

**Anna:** There are several ways to classify fuzzers. The survey by Li, Zhao and Zhang that we used for this section describes many of them. We'll look at four.

**Anna:** Mutation-based fuzzing starts from valid input and changes, deletes or inserts values. For example it takes 25 and turns it into 2-5. It shows how the program copes with corrupted input.

**Anna:** Generation-based fuzzing builds inputs from scratch from rules or a specification. That's useful for structured formats such as JSON, XML and protocols, where random bytes would be rejected immediately.

**Anna:** Dumb fuzzing sends random or malformed input without knowing the expected format. It's simple, but weak on complex systems. Smart fuzzing uses knowledge of the input structure, format or protocol, or feedback from the program, to create more meaningful test cases and reach deeper bugs.

**Anna:** The fuzzer we use today, AFL++, is mutation-based, and its 'smartness' comes from feedback about which code each input reaches. Francesco will explain how that works, but first, how does fuzzing fit with testing you already know?

## 4. Script, slide by slide (part 2: tools, exercises, demo, 0:20 to 1:12)

### Slide 7: How is fuzz testing different? (Francesco, 4 min)

**Francesco:** Thanks Anna. You all know unit, integration and system testing, so let's compare. Who has written a unit test? [Hands.] Good.

**Francesco:** Unit testing checks individual functions or components with cases the developer defines. Integration testing checks components working together, so the interactions between parts. System testing checks the whole application, end to end. Fuzz testing explores unexpected or malformed inputs and searches for failures nobody anticipated.

**Francesco:** Look at the arrow: on the left the behaviour is expected and designed, on the right we look for the unexpected. A unit test only finds bugs you thought of, because you wrote the test case. A fuzzer finds the ones you didn't think of. And the line at the bottom matters: fuzz testing complements traditional testing, it doesn't replace it. A fuzzer can tell you the program crashed, but not that a discount was calculated wrongly.

### Slide 8: Fuzzing tools landscape (Francesco, 4 min) NEW

| Tool | Typical target | Idea |
| --- | --- | --- |
| AFL++ | C and C++ programs | Coverage-guided mutation fuzzing; the one we use |
| libFuzzer | Library functions in C and C++ | In-process, coverage-guided, part of LLVM |
| Jazzer | Java and JVM code | Coverage-guided fuzzing for the JVM |
| Atheris | Python code | Coverage-guided fuzzing for Python |
| Hypothesis | Python code | Property-based testing that generates inputs and shrinks failures |
| OWASP ZAP, Burp Suite | Web applications | Fuzz request parameters and forms |
| OSS-Fuzz | Open-source projects | Google's continuous fuzzing service |

**Francesco:** So what do people actually use? There are many fuzzers, and the choice depends on what you test. For C and C++ programs, AFL and its community fork AFL++ are the best known, and libFuzzer is another popular one that fuzzes library functions inside the process. For other languages you have Jazzer for Java and Atheris for Python. Hypothesis, also for Python, is property-based testing, which is a close cousin of fuzzing. For web applications, tools like OWASP ZAP and Burp Suite have fuzzing features. And OSS-Fuzz is a Google service that keeps fuzzing open-source projects around the clock.

**Francesco:** The tools differ in language and in how clever they are, but the idea is the same everywhere: generate odd input, run, watch for failures, save the input that failed. We picked AFL++ because it's open source, widely used, and easy to run in Docker.

### Slide 9: How AFL++ works (Francesco, 4 min) NEW

**Slide content:** a loop diagram with five boxes. (1) Seed inputs. (2) Mutate: flip bits, change bytes, insert, delete, splice. (3) Run the instrumented program. (4) Reached new code? Keep the input. (5) Crashed or hung? Save the input. Arrow from 4 back to 2. Footer: 'Compile with afl-cc to add instrumentation'.

**Francesco:** Let me explain what happens when we start AFL++, because it makes the demo much easier to follow. First you compile the program with AFL++'s compiler, `afl-cc`. It adds small markers that record which parts of the code ran for each input. That feedback is called coverage.

**Francesco:** Then you give it one or more seed inputs, valid examples of what the program reads. AFL++ mutates a seed: it flips bits, changes bytes, inserts and deletes pieces. It runs the program with the new input. If the input made the program reach new code, AFL++ keeps it and mutates that one in later rounds. If the program crashes, AFL++ saves that input as a crash. Then it repeats, thousands of times per second.

**Francesco:** So it learns which inputs are interesting without reading the source code. This is called coverage-guided greybox fuzzing: grey, because it looks inside the program a little, through the instrumentation, but doesn't need a full understanding of it. The name AFL stands for 'American fuzzy lop', which is a breed of rabbit.

### Slide 10: Exercise 1, become the fuzzer (Anna starts, both, 8 min)

**Anna:** Now it's your turn to be the fuzzer. Work in pairs for 3 minutes. On the screen is a test application with a username field and an age field. Your mission has three steps: think, break, record. Think of unusual inputs, try to trigger unexpected behaviour, and write down anything interesting.

**Francesco:** To help you, think about three things. Format: what if it's text instead of a number, or contains special characters? Length: what if it's empty or a million characters long? Value: what if it's zero, negative, huge, or right on a boundary like 149, 150 and 151? Start now.

[3 minutes. Walk around and ask pairs what they have.]

**Anna:** Time. Who has the weirdest input? [Write answers on the board in three columns: format, length, value. Take 4 minutes.]

**Francesco:** Notice what you did: you generated inputs, tried them, watched for strange behaviour and recorded the interesting ones. That's the fuzz testing process from slide 5, done by hand. You found a handful in three minutes. Now let's see a fuzzer do it.

### Slide 11: Live demo with AFL++ (Francesco drives, Anna narrates, 8 min) NEW

**Slide content:** the 12 lines of `demo.c` on the left and the five commands from section 2 on the right. Keep a terminal open next to the slide.

**Anna:** This is our target program. It reads one character from standard input, and if that character is a capital X it calls abort(), which crashes the program. Otherwise it exits normally. Here is a question for you: what input crashes it? [Pause.] Yes, X. That's easy to see in 12 lines. Now imagine a million lines, where nobody knows which input is the dangerous one. So let's pretend we don't know, and let the fuzzer find it.

**Francesco:** [Run `docker compose run --rm workshop`.] We're inside the container. First I compile the program with AFL++'s compiler. [Run `afl-cc exercises/demo/demo.c -o demo`.] This adds the instrumentation I described on the last slide.

**Francesco:** Now a seed input. [Run `mkdir -p /tmp/in && printf 'A' > /tmp/in/seed`.] The seed is just the letter A, which is harmless. Now we start the fuzzer. [Run `afl-fuzz -i /tmp/in -o /tmp/out -- ./demo`.]

**Anna:** While it runs, look at the screen. 'Total execs' is how many times it ran our program, and 'exec speed' is how fast. It's thousands per second. Look for 'saved crashes'. [Wait for the counter to go to 1, usually within seconds.] There: it found a crash.

**Francesco:** [Stop with Ctrl+C.] Let's look at what it found. [Run `ls /tmp/out/default/crashes`, set `f`, then `od -c "$f"`.] The file name contains 'sig:06', which is signal 6, the abort signal. And inside the file, the first character is an X. The fuzzer found the one input out of 256 possible first bytes that breaks the program, without us telling it.

**Francesco:** And the best part is that it saved the input. [Run `./demo < "$f"; echo $?`.] We can replay it at any time: the program aborts, and the exit code is 134. That's the reproducibility advantage we'll come back to.

**Anna:** One more point: this fuzzer only knew the program crashed. It had no idea that crashing was 'wrong' in any deeper sense. We'll come back to that in the limitations. Now let's take a 4 minute break. If your Docker image isn't built yet, come to us.

### Break, 0:48 to 0:52

[Both: help anyone with Docker problems. Keep the commands from slide 12 on screen.]

### Slide 12: Exercise 2, find the crash (Anna starts, both circulate, 20 min) NEW

**Slide content:** title 'Find the crash', the tasks below, and the line 'No peeking at exercises/participant/participant.c until task 5!'.

1. Start the container and compile: `afl-cc exercises/participant/participant.c -o exercise`.
2. Create a seed: `mkdir -p /tmp/in2 && printf 'hi' > /tmp/in2/seed`.
3. Fuzz: `afl-fuzz -i /tmp/in2 -o /tmp/out2 -- ./exercise`. Let it find a crash, then Ctrl+C.
4. Look at the files in `/tmp/out2/default/crashes`. What do they have in common? Guess the rule that makes the program crash. Check sizes with `wc -c`.
5. Now open `exercises/participant/participant.c` and check your guess.
6. Shrink a crash with `afl-tmin -i "$f" -o /tmp/min -- ./exercise`. How big is the smallest crashing input?
7. Stretch: change the condition in `exercises/participant/participant.c` (for example, also require the first byte to be 'F'), recompile, and fuzz again. Is it still found, and how fast? Or run AFL++ a second time with a different seed and compare how fast it finds a crash.

**Anna:** For the next 20 minutes you'll fuzz a second program, but this time you don't get to read the source first. The task is on the slide. Run AFL++ on it, look at the crashes it saves, and from those crash files alone, work out what rule makes the program crash. Only then open the code and check your guess. The commands are on screen and in the participant README.

**Francesco:** Fast pairs: when you're done, do the stretch task. We'll walk around, so put your hand up if you're stuck.

[Circulate. Questions to ask pairs:]

- 'What do all your crash files have in common?'
- 'How long is the shortest crash file, and how long is the smallest one after afl-tmin?'
- 'What would a unit test have needed to find this?'

[Give a 3-minute warning at 1:09.]

**Francesco:** Let's wrap up. The program crashes whenever it reads 8 or more bytes. Who guessed that from the crash files? [Hands.] And what did afl-tmin give you? [Expect: 8 bytes.] The fuzzer found a crash and then shrank it to the smallest input that still breaks the program, which is exactly what you want to paste into a bug report.

## 5. Script, slide by slide (part 3: wrap-up, 1:12 to 1:30)

### Slide 13: Why use fuzz testing? (Francesco, 3 min)

**Francesco:** So why would you use fuzz testing? Four reasons, and you've just seen all of them. First, it finds the unexpected: problems developers may not have anticipated, which is why a hand-written test list will always miss some. Second, automation: look at the 'exec speed' you saw in the demo. Large numbers of inputs, thousands or even millions, can be generated and tested without anyone sitting there. Third, security: malformed input can expose crashes, unsafe behaviour and vulnerabilities, which is why fuzzing is so popular for file parsers and network code. The OWASP community page lists this too. Fourth, reproducibility: the failing input was saved, we replayed it, and afl-tmin shrank it. That makes debugging easier and prevents the bug returning.

**Francesco:** And again: another tool in the toolbox, not a replacement.

### Slide 14: Fuzz testing limitations (Anna, 4 min)

**Anna:** Fuzzing isn't magic, and the OWASP material is clear about that. First, it can miss logical bugs. Think about what our fuzzer saw: only whether the program crashed. If a program returned the wrong answer without crashing, AFL++ would never notice. Second, time and resources: running thousands or millions of cases needs computing power and time. Third, false positives and noisy results: some failures aren't serious and still need investigating.

**Anna:** Fourth, it requires setup: a fuzzer needs a suitable target, input format and configuration. You saw that we had to compile with a special compiler and prepare a seed. Fifth, limited input understanding: simple random fuzzing may generate inputs that are rejected immediately and never reach the deeper parts of the application, which is why smarter fuzzers use feedback like the coverage in AFL++.

**Anna:** A question for you: which of these did you notice during the exercise? [Take one or two answers.]

### Slide 15: Kahoot quiz (Anna, 7 min)

**Anna:** Time to see what stuck. Open kahoot.it on your phone or laptop and enter the PIN on the screen. Use a nickname, nothing offensive please. There are eight questions, faster correct answers get more points, and the winner gets glory.

[Run the quiz. Francesco shows the leaderboard and names the top three.]

**Francesco:** Well done. [Name the winners.]

### Slide 16: Conclusions (both, 3 min) NEW

**Slide content:** four takeaways. (1) Fuzzing feeds random, invalid or unexpected input and watches for failures. (2) Mutation or generation based, dumb or smart; AFL++ is coverage-guided. (3) It complements unit, integration and system tests, it doesn't replace them. (4) Saved inputs make bugs reproducible. Also the repo link.

**Francesco:** Let's sum up. Fuzzing means feeding a program unexpected input and watching for failures. There are different kinds, and the smarter ones use feedback from the program, like AFL++ did today.

**Anna:** It complements the testing you already do, because it finds the bugs you didn't think of, but it needs a way to recognise failure and it can't judge business logic.

**Francesco:** And you've done it: you broke an application by hand, watched AFL++ find a crash in seconds, then found and shrank a crash yourselves. Next time you write code that reads a file, a network message or a form field, ask yourself: what happens if this input is garbage?

### Slide 17: Sources and questions (both, 1 min) NEW

**Anna:** Our sources are on the screen and in the README of the repository, together with all the code from today.

**Francesco:** We'd love your feedback: what worked and what we could improve. Thank you for taking part. Any questions?