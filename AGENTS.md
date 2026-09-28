# Pumptools contributor instructions

These instructions apply throughout the repository. Read the relevant source,
tests, and documentation before changing behavior. See `CONTRIBUTING.md`,
`doc/development/development.md`, and `doc/development/architecture.md` for the
broader contributor guidance.

## Engineering principles

- Follow the patterns in neighboring code for naming, ownership, lifecycle,
  error handling, logging, documentation, and CMake organization.
- Keep each responsibility in the narrowest existing module. Reusable
  corrections belong in a suitable patch module; game hooks select and
  initialize the modules required by that game.
- Preserve API return values and `errno` behavior. Use module-scoped logging for
  runtime failures and assertions for internal invariants.
- Preserve pass-through behavior when a hook does not own or recognize a call.
- Treat constructors, destructors, `__libc_start_main`, `dlopen`, `dlsym`,
  `RTLD_NEXT`, signatures, calling conventions, hard-coded addresses,
  initialization and shutdown order, recursion, and thread safety as
  compatibility-sensitive boundaries.
- Follow `.clang-format` for maintained C and header files. Inspect the focused
  diff before running `make clang-format`, and do not format vendored code under
  `src/imports/`. Follow the established local style for CMake, Make, shell, and
  Markdown files.
- Comments and documentation should explain non-obvious constraints, ABI or
  lifecycle requirements, and reasons. Update user or development documentation
  when configuration, public APIs, setup, or supported behavior changes.

## Tests and hooked behavior

- Add or update unit tests whenever changed behavior can be isolated with the
  existing test framework. For a bug fix, add a regression case that
  demonstrates the previous failure where practical.
- Preserve coverage for existing behavior while testing the new expectation.
  Include relevant success, failure, malformed-input, disabled, no-match, and
  pass-through cases.
- Check observable behavior as applicable: parameters, calls, buffers, return
  values, `errno`, side effects, lifecycle, and ownership. Do not merely relax
  an expectation to accommodate a changed implementation.
- Put production behavior in the production module and exercise that same code
  from tests. Do not create a test-only copy of parsing or decision logic.
- For hook behavior, use the capnhook named-function-mock seam and CMocka when
  the dependency can be isolated. Verify whether and how the original function
  is called, then keep the real game-hook path responsible for initializing the
  tested patch module.
- `src/main/hook/propatch/usb-fix.c` and
  `src/test/hook/propatch/usb-fix/main.c` provide the established pattern for a
  production patch integrated with mocked detoured calls.
- Register new tests in the corresponding `cmake/src/test/` hierarchy.

## Verification

Run the applicable checks from the repository root:

```sh
make build
make test
make build-docker
```

Compilation, unit tests, Docker compatibility builds, fixture or process tests,
game-runtime tests, and cabinet or hardware tests are separate evidence levels.
Do not claim game or hardware compatibility from compilation or unit tests.
State what was actually tested and the relevant platform, game, and hardware
details.

## Maintainer interaction and LLM use

Follow the LLM policy in `CONTRIBUTING.md`. Human contributors remain
responsible for understanding, reviewing, testing, and validating submitted
work.

Do not autonomously publish issues, pull requests, reviews, comments, or
replies. Prepare concise drafts for human review. Filter analysis and tool
output to evidence relevant to the discussion rather than transferring raw or
excessive output to maintainers. A human must choose what to communicate,
provide its context, guide the discussion, and respond to maintainer feedback.

Only describe work as an unvetted or deliberately rough prototype when the
maintainers have agreed to receive it on that basis. Clearly state its scope,
limitations, and verification status.

## Sensitive material

Do not read, reproduce, log, or commit credentials, game assets, dongle keys,
raw security material, private identifiers, or unsanitized runtime evidence.
