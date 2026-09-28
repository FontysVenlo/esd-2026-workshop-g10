# Fuzz Testing Workshop - Francesco

## Responsibilities

My responsibilities for the Fuzz Testing workshop are:

- Welcome and Icebreaker
- First part of the Introduction to Fuzz Testing
- Research into the advantages of Fuzz Testing
- Preparation of practical exercises

To do still: 
-Create a powerpoint

---

## 1. Welcome and Icebreaker

### Icebreaker Idea - "How would you break this?"

Present the participants with a simple input field or function and ask:

> **"If your goal was to break this application, what would you enter?"**

For example:

```text
Enter your age: ______
```

Possible answers:

```text
-1
999999999999999999
hello
<empty input>
!@#$%^&*
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa...
```

After collecting several answers, explain that developers normally test
inputs they expect, while fuzz testing can systematically generate unexpected,
invalid or malformed inputs to discover behaviour that developers may not
have anticipated.

This provides a natural transition from the icebreaker into the introduction
to fuzz testing.

---

## 2. Introduction to Fuzz Testing - Part 1

### 2.1 What is Fuzz Testing?

Fuzz testing, also called **fuzzing**, is an automated software testing
technique in which a program is repeatedly executed using generated or
mutated inputs.

The objective is to discover unexpected program behaviour, such as:

- Crashes
- Exceptions
- Hangs or infinite loops
- Memory-related errors
- Security vulnerabilities
- Incorrect handling of malformed input

Instead of manually defining every possible test case, a **fuzzer**
automatically explores a large input space and observes how the target
application behaves.

### 2.2 Basic Fuzzing Process

A simplified fuzzing process can be represented as:

```text
        Generate / Mutate Input
                 |
                 v
            Run Program
                 |
                 v
        Observe Behaviour
                 |
          +------+------+
          |             |
      No Failure      Failure
          |             |
          v             v
     New Input      Save Input
          |             |
          +------> Repeat
```

When an input causes a failure, the input can be saved so that developers
can reproduce and investigate the problem.

### 2.3 Simple Example

Consider a simple function:

```python
def process_age(age):
    if age >= 18:
        return "Adult"
    return "Minor"
```

Traditional tests might use expected values such as:

```text
17
18
25
60
```

However, unexpected inputs could include:

```text
-100
0
999999999
"hello"
""
None
```

A fuzzing approach attempts to automate this process by generating or
mutating large numbers of inputs and monitoring how the application responds.

The important idea is that fuzzing is not simply about using random values.
Its purpose is to automatically explore unexpected parts of the input space
that developers may not have considered when creating traditional tests.

---

## 3. Advantages of Fuzz Testing

### 3.1 Finding Unexpected Bugs

One of the main advantages of fuzz testing is its ability to discover
problems that developers did not explicitly anticipate when writing
traditional test cases.

Developers generally create tests based on expected behaviour and known
edge cases. Fuzzing can explore unusual and malformed inputs that may reveal
previously unknown problems.

### 3.2 Automation

Fuzz testing can automatically execute a large number of test cases.

Once the fuzzing environment has been configured, the fuzzer can continuously
generate inputs, execute the target program and monitor its behaviour.

This allows considerably more inputs to be explored than would normally be
practical through manual testing.

### 3.3 Security Testing

Fuzzing can be particularly useful in security testing because malformed or
unexpected inputs may expose vulnerabilities or unsafe behaviour.

For example, fuzzing may help identify:

- Unexpected crashes
- Memory corruption
- Incorrect input validation
- Parser errors
- Unexpected exceptions

### 3.4 Reproducibility

When fuzzing discovers an input that causes a failure, the failing input can
be stored.

Developers can then use this input to:

1. Reproduce the failure
2. Investigate its cause
3. Implement a fix
4. Test the application again

This makes discovered failures useful for debugging and regression testing.

### 3.5 Complements Traditional Testing

Fuzz testing should not be considered a replacement for other testing
techniques such as unit testing or integration testing.

Instead, fuzzing can complement traditional testing by focusing on unexpected
inputs and behaviours that manually designed tests may not cover.

### Research Questions

The following questions should be investigated further and supported with
reliable sources for the final workshop:

- What types of software defects are fuzzers particularly effective at finding?
- What advantages does fuzzing provide compared with manually written tests?
- Why is fuzzing commonly used in security testing?
- What are examples of real vulnerabilities discovered through fuzzing?
- What are the main limitations of fuzz testing?
- When should fuzz testing be introduced into the development/testing process?

---

## 4. Practical Exercises

### Exercise 1 - Manual Fuzzer

Before introducing an automated fuzzing tool, participants receive a small
function or input field and are asked to create inputs intended to break it.

Example:

```text
Username: __________
Age:      __________
```

Participants should think of unusual values such as:

```text
Empty input
Extremely long input
Negative numbers
Very large numbers
Special characters
Unexpected text
```

After several minutes, the generated inputs can be compared between groups.

The exercise can then transition into automated fuzzing:

> "We managed to think of several strange inputs ourselves. But what if we
> wanted to test thousands or millions of different inputs?"

---

### Exercise 2 - First Automated Fuzzer

Participants run a prepared fuzzing example against a deliberately buggy
function or application.

The basic workflow is:

```text
1. Inspect the target program
        |
        v
2. Predict possible failures
        |
        v
3. Run the fuzzer
        |
        v
4. Observe generated inputs
        |
        v
5. Discover a failure
        |
        v
6. Inspect the failing input
        |
        v
7. Reproduce the failure manually
```

#### Goal

Participants should understand the relationship between:

**Input generation -> Program execution -> Failure detection -> Reproduction**

The exercise should demonstrate how an automated fuzzer can explore inputs
much faster than manually creating individual test cases.

---

### Exercise 3 - Find the Bug Challenge

Participants receive a slightly more complicated target without being told
where the bug is located.

They must:

1. Run the fuzzer
2. Discover a failing input
3. Reproduce the failure
4. Identify why the input causes the problem
5. Suggest or implement a fix
6. Run the fuzzer again

#### Goal

This exercise demonstrates that finding a crash is only the beginning of the
process.

Fuzzing results must still be investigated by developers to understand the
underlying problem and determine how it should be fixed.

---

## 5. Workshop Preparation

### Research

- [ ] Find reliable academic/technical sources defining fuzz testing
- [ ] Research and substantiate the main advantages of fuzz testing
- [ ] Research the limitations of fuzz testing
- [ ] Find examples of vulnerabilities discovered through fuzzing
- [ ] Research how fuzzing complements traditional software testing
- [ ] Research the different types of fuzzing if relevant to the introduction

### Exercises

- [ ] Decide which programming language will be used
- [ ] Decide which fuzzing tool/framework will be used
- [ ] Create the first simple fuzzing example
- [ ] Create a deliberately buggy target for the exercises
- [ ] Create the "Find the Bug" challenge
- [ ] Test all exercises beforehand
- [ ] Estimate the required time for each exercise
- [ ] Prepare backup failing inputs in case fuzzing takes too long during the workshop

### Coordination

- [ ] Coordinate Introduction Part 1 with Introduction Part 2
- [ ] Avoid duplicated theoretical content between presenters
- [ ] Decide when the practical exercises will take place
- [ ] Decide which presenter will assist participants during each exercise
- [ ] Finalise the overall workshop timing