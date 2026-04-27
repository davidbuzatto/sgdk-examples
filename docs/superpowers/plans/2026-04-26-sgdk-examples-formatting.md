# SGDK Examples — Code Formatting & Comment Translation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Reformat all 29 `src/main.c` files across the SGDK examples project to match David Buzatto's coding style and translate all Spanish comments to English.

**Architecture:** Each task covers one lesson group. Files are read, transformed in place following the formatting rules below, then committed. No logic changes — only formatting, comment translation, and text string translation (on-screen labels).

**Tech Stack:** C (SGDK), manual file editing via Claude Code tools.

---

## Formatting Rules Reference

These rules are derived from the `fighting-game` project and must be applied uniformly to every file.

### 1. Header Comment

Replace the current `@Title`/`@Author` block with a Doxygen block using `@file`, `@author`, and `@brief`. Keep the original author and add a `@note` crediting the reimplementation:

```c
// BEFORE
/**
 *      @Title:  Leccion 01 - "Hello world"
 *      @Author: Daniel Bustos "danibus"
 */

// AFTER
/**
 * @file main.c
 * @author Daniel Bustos "danibus"
 * @brief Lesson 01 - "Hello World" example for SGDK.
 *
 * @note Reimplemented and updated to the latest SGDK API by
 *       Prof. Dr. David Buzatto.
 */
```

### 2. Indentation

- **4 spaces** per level. No tabs.
- Every nested block gets one more level.

### 3. Spaces Inside Parentheses

ALL parenthesised expressions get inner spaces — function calls, casts, conditions:

```c
// BEFORE
PAL_setPalette(PAL0, my64tiles.palette->data, CPU);
if (value & BUTTON_LEFT)

// AFTER
PAL_setPalette( PAL0, my64tiles.palette->data, CPU );
if ( value & BUTTON_LEFT )
```

### 4. Always Use Braces

Every `if`, `else`, `for`, `while` body gets braces, even single-statement ones:

```c
// BEFORE
if (value & BUTTON_LEFT)    VDP_drawText("LEFT", 8, 10);

// AFTER
if ( value & BUTTON_LEFT ) {
    VDP_drawText( "LEFT", 8, 10 );
}
```

### 5. Multi-Condition `if` Formatting

Each condition on its own line, aligned under the opening `(`:

```c
// BEFORE
if ((!(value & BUTTON_UP))   && (!(value & BUTTON_DOWN)) &&(!(value & BUTTON_LEFT)) &&
    (!(value & BUTTON_RIGHT))&& (!(value & BUTTON_A))    &&(!(value & BUTTON_B))    &&
    ...
    VDP_clearTextArea(0,7,40,28);

// AFTER
if ( !( value & BUTTON_UP )    && !( value & BUTTON_DOWN )  &&
     !( value & BUTTON_LEFT )  && !( value & BUTTON_RIGHT ) &&
     !( value & BUTTON_A )     && !( value & BUTTON_B )     &&
     !( value & BUTTON_C )     && !( value & BUTTON_X )     &&
     !( value & BUTTON_Y )     && !( value & BUTTON_Z )     &&
     !( value & BUTTON_MODE )  && !( value & BUTTON_START ) ) {
    VDP_clearTextArea( 0, 7, 40, 28 );
}
```

### 6. Spacing Around Operators

Space on both sides of every binary operator (`=`, `&`, `+`, `-`, `|`, `==`, `!=`, etc.):

```c
// BEFORE
ind += my64tiles.tileset->numTile;
u16 ind;

// AFTER  (unchanged — already correct)
ind += my64tiles.tileset->numTile;
u16 ind;
```

### 7. Blank Line After Opening / Before Closing Brace of Functions

```c
// AFTER
int main( bool hard ) {

    VDP_drawText( "Hello World!", 10, 13 );

    while ( TRUE ) {

        SYS_doVBlankProcess();

    }

    return 0;

}
```

### 8. Blank Line Between Variable Declarations and Code

```c
static void handleInput( void ) {

    u16 value = JOY_readJoypad( JOY_1 );

    if ( value & BUTTON_LEFT ) {
        ...
    }

}
```

### 9. Comment Translation

- Translate **all** Spanish/Portuguese comments to English.
- Remove or replace garbled encoding artifacts (`sint�cticamente`, `posici�n`, etc.) — these appear where accented characters were corrupted. Replace with clean English equivalents.
- Example:
  ```c
  // BEFORE: //leer controles, se llama siempre una vez por frame
  // AFTER:  // read controls, called once per frame
  ```

### 10. On-Screen Text Strings

Translate Spanish display strings to English:

```c
// BEFORE
VDP_drawText("Controles: FORMA SINCRONA", 6, 4);
VDP_drawText("IZQUIERDA", 8, 10);

// AFTER
VDP_drawText("Controls: SYNCHRONOUS MODE", 6, 4);
VDP_drawText("LEFT", 8, 10);
```

---

## Task 1: Lesson 01 — Hello World

**File:** `lesson-01-hello-world/src/main.c` (29 lines)

**Issues:** Spanish comments, garbled encoding in lines 24-26, header format.

- [ ] **Step 1: Read the file**

  Read `lesson-01-hello-world/src/main.c`.

- [ ] **Step 2: Write the reformatted file**

  Apply all rules. The complete result must be:

  ```c
  /**
   * @file main.c
   * @author Daniel Bustos "danibus"
   * @brief Lesson 01 - "Hello World" example for SGDK.
   *
   * @note Reimplemented and updated to the latest SGDK API by
   *       Prof. Dr. David Buzatto.
   */

  #include <genesis.h>

  int main( bool hard ) {

      // VDP_drawText( "text", x, y ) — (x, y) refers to TILES, not pixels (1 tile = 8 pixels)
      VDP_drawText( "Hello World!", 10, 13 );

      while ( TRUE ) {

          // read controls
          // move sprites
          // update scores
          // etc.

          SYS_doVBlankProcess(); // waits for the next Vertical Blanking start

      }

      // main() returns int, so it needs a return statement to be syntactically correct.
      // However, we should NEVER reach this point — a console game never ends.
      // If reached, an "ILLEGAL INSTRUCTION" error will occur.
      return 0;

  }
  ```

- [ ] **Step 3: Commit**

  ```bash
  git add lesson-01-hello-world/src/main.c
  git commit -m "style: reformat lesson-01 and translate comments to English"
  ```

---

## Task 2: Lesson 02 — Controls (Synchronous & Asynchronous)

**Files:**
- `lesson-02-controls/src/main.c` (76 lines)
- `lesson-02-controls-asynchronous/src/main.c` (81 lines)

**Issues:** Spanish comments and on-screen strings, unbraced single-line `if`s with extra alignment spaces, complex multi-condition `if` without braces, garbled encoding on line 69.

- [ ] **Step 1: Read both files**

  Read `lesson-02-controls/src/main.c` and `lesson-02-controls-asynchronous/src/main.c`.

- [ ] **Step 2: Reformat `lesson-02-controls/src/main.c`**

  Full expected result after transformation:

  ```c
  /**
   * @file main.c
   * @author Daniel Bustos "danibus"
   * @brief Lesson 02 - "Controls" (synchronous mode) example for SGDK.
   *
   * @note Reimplemented and updated to the latest SGDK API by
   *       Prof. Dr. David Buzatto.
   */

  #include <genesis.h>

  // function declarations
  static void handleInput( void );

  int main( bool hard ) {

      VDP_drawText( "Controls: SYNCHRONOUS MODE", 6, 4 );

      while ( TRUE ) {

          // read controls — called once per frame
          handleInput();

          SYS_doVBlankProcess();

      }

      return 0;

  }

  static void handleInput( void ) {

      // store the current joypad state
      u16 value = JOY_readJoypad( JOY_1 );

      if ( value & BUTTON_LEFT ) {
          VDP_drawText( "LEFT", 8, 10 );
      }

      if ( value & BUTTON_RIGHT ) {
          VDP_drawText( "RIGHT", 20, 10 );
      }

      if ( value & BUTTON_UP ) {
          VDP_drawText( "UP", 16, 8 );
      }

      if ( value & BUTTON_DOWN ) {
          VDP_drawText( "DOWN", 16, 12 );
      }

      if ( value & BUTTON_A ) {
          VDP_drawText( "BUTTON A", 6, 18 );
      }

      if ( value & BUTTON_B ) {
          VDP_drawText( "BUTTON B", 14, 18 );
      }

      if ( value & BUTTON_C ) {
          VDP_drawText( "BUTTON C", 22, 18 );
      }

      if ( value & BUTTON_X ) {
          VDP_drawText( "BUTTON X", 6, 20 );
      }

      if ( value & BUTTON_Y ) {
          VDP_drawText( "BUTTON Y", 14, 20 );
      }

      if ( value & BUTTON_Z ) {
          VDP_drawText( "BUTTON Z", 22, 20 );
      }

      if ( value & BUTTON_START ) {
          VDP_drawText( "START", 10, 16 );
      }

      if ( value & BUTTON_MODE ) {
          VDP_drawText( "MODE", 20, 16 );
      }

      // if no button is pressed, clear the area below the title
      // VDP_clearTextArea() works in tiles
      if ( !( value & BUTTON_UP )    && !( value & BUTTON_DOWN )  &&
           !( value & BUTTON_LEFT )  && !( value & BUTTON_RIGHT ) &&
           !( value & BUTTON_A )     && !( value & BUTTON_B )     &&
           !( value & BUTTON_C )     && !( value & BUTTON_X )     &&
           !( value & BUTTON_Y )     && !( value & BUTTON_Z )     &&
           !( value & BUTTON_MODE )  && !( value & BUTTON_START ) ) {
          VDP_clearTextArea( 0, 7, 40, 28 );
      }

  }
  ```

- [ ] **Step 3: Reformat `lesson-02-controls-asynchronous/src/main.c`**

  Apply the same rules as Step 2 (the asynchronous variant uses a callback instead of polling; translate callback-related Spanish comments like "declaración de función de manejo de entrada" → "input handler callback declaration", "registrar el manejador" → "register the input handler").

- [ ] **Step 4: Commit**

  ```bash
  git add lesson-02-controls/src/main.c lesson-02-controls-asynchronous/src/main.c
  git commit -m "style: reformat lesson-02 files and translate comments to English"
  ```

---

## Task 3: Lesson 03 — Tiles and Planes

**Files:**
- `lesson-03-tiles-and-planes-1/src/main.c` (56 lines)
- `lesson-03-tiles-and-planes-2/src/main.c` (202 lines)

**Issues:** Spanish comments, missing spaces inside parentheses, missing braces on some `if`s.

- [ ] **Step 1: Read both files**

  Read `lesson-03-tiles-and-planes-1/src/main.c` and `lesson-03-tiles-and-planes-2/src/main.c`.

- [ ] **Step 2: Reformat `lesson-03-tiles-and-planes-1/src/main.c`**

  Apply all rules:
  - Update header (`@brief Lesson 03 - "Tiles and Planes (1)"`)
  - Add spaces inside all function call parentheses
  - Add braces to any unbraced control structures
  - Translate all Spanish comments to English
  - Translate on-screen Spanish strings to English

- [ ] **Step 3: Reformat `lesson-03-tiles-and-planes-2/src/main.c`**

  Same rules. This is a longer file (202 lines) — pay attention to:
  - Blank lines between each variable declaration block and the following code
  - Any inconsistent indentation in nested loops

- [ ] **Step 4: Commit**

  ```bash
  git add lesson-03-tiles-and-planes-1/src/main.c lesson-03-tiles-and-planes-2/src/main.c
  git commit -m "style: reformat lesson-03 files and translate comments to English"
  ```

---

## Task 4: Lesson 04 — Tiles from Images

**Files:**
- `lesson-04-tiles-from-images-1/src/main.c` (32 lines)
- `lesson-04-tiles-from-images-2/src/main.c` (33 lines)
- `lesson-04-tiles-from-images-3/src/main.c` (33 lines)
- `lesson-04-tiles-from-images-4/src/main.c` (128 lines)

**Issues:** Mixed tabs/spaces on lines 15-16 and 22-25 (files 2, 3, 4), garbled accented characters, Spanish comments.

- [ ] **Step 1: Read all four files**

  Read each of the four `main.c` files.

- [ ] **Step 2: Reformat `lesson-04-tiles-from-images-1/src/main.c`**

  Apply all rules:
  - Update header (`@brief Lesson 04 - "Tiles from Images (1)"`)
  - Add spaces inside parentheses
  - Translate comments (e.g., `//para llevar la cuenta de tiles en VRAM` → `// tile counter in VRAM`)
  - Fix any garbled characters

- [ ] **Step 3: Reformat `lesson-04-tiles-from-images-2/src/main.c`**

  Same rules as Step 2. Also fix the mixed tab/space indentation on lines 15-16 — convert to 4 spaces.

- [ ] **Step 4: Reformat `lesson-04-tiles-from-images-3/src/main.c`**

  Same as Step 3 — fix tab/space mix on lines 15-16, 22-25.

- [ ] **Step 5: Reformat `lesson-04-tiles-from-images-4/src/main.c`**

  Same rules. Fix mixed indentation on lines 15-16, 25, 45, 47-48.

- [ ] **Step 6: Commit**

  ```bash
  git add lesson-04-tiles-from-images-1/src/main.c lesson-04-tiles-from-images-2/src/main.c lesson-04-tiles-from-images-3/src/main.c lesson-04-tiles-from-images-4/src/main.c
  git commit -m "style: reformat lesson-04 files and translate comments to English"
  ```

---

## Task 5: Lesson 05 — Sprites

**Files:**
- `lesson-05-sprites-1/src/main.c` (112 lines)
- `lesson-05-sprites-2-animated/src/main.c` (128 lines)
- `lesson-05-sprites-3-enemies/src/main.c` (184 lines)

**Issues:** Spanish comments, UTF-8 encoding issues (lines 8, 160 of sprites-3), mixed indentation on sprites-2 (lines 8, 48).

- [ ] **Step 1: Read all three files**

  Read each of the three `main.c` files.

- [ ] **Step 2: Reformat `lesson-05-sprites-1/src/main.c`**

  Apply all rules:
  - Update header (`@brief Lesson 05 - "Sprites (1)"`)
  - Spaces inside parentheses
  - Braces on all control structures
  - Translate Spanish comments

- [ ] **Step 3: Reformat `lesson-05-sprites-2-animated/src/main.c`**

  Same rules. Also fix the mixed indentation on lines 8 and 48 — convert to 4 spaces.

- [ ] **Step 4: Reformat `lesson-05-sprites-3-enemies/src/main.c`**

  Same rules. Replace corrupted UTF-8 characters on lines 8 and 160 with clean English equivalents.

- [ ] **Step 5: Commit**

  ```bash
  git add lesson-05-sprites-1/src/main.c lesson-05-sprites-2-animated/src/main.c lesson-05-sprites-3-enemies/src/main.c
  git commit -m "style: reformat lesson-05 files and translate comments to English"
  ```

---

## Task 6: Lesson 06 — Debug

**File:** `lesson-06-debug-1/src/main.c` (426 lines)

**Issues:** Spanish comments throughout a long file, consistent formatting but needs spaces inside parentheses and braces on single-line ifs.

- [ ] **Step 1: Read the file**

  Read `lesson-06-debug-1/src/main.c`.

- [ ] **Step 2: Reformat the file**

  Apply all rules:
  - Update header (`@brief Lesson 06 - "Debug (1)"`)
  - Spaces inside all function-call and condition parentheses
  - Braces on every control structure body
  - Translate all Spanish comments to English
  - Translate any Spanish on-screen strings to English

- [ ] **Step 3: Commit**

  ```bash
  git add lesson-06-debug-1/src/main.c
  git commit -m "style: reformat lesson-06 and translate comments to English"
  ```

---

## Task 7: Lesson 07 — Deconstructing Scenarios

**File:** `lesson-07-deconstructing-scenarios/src/main.c` (49 lines)

**Issues:** Spanish comments, missing spaces inside parentheses.

- [ ] **Step 1: Read the file**

  Read `lesson-07-deconstructing-scenarios/src/main.c`.

- [ ] **Step 2: Reformat the file**

  Apply all rules:
  - Update header (`@brief Lesson 07 - "Deconstructing Scenarios"`)
  - Spaces inside parentheses
  - Braces on control structures
  - Translate comments

- [ ] **Step 3: Commit**

  ```bash
  git add lesson-07-deconstructing-scenarios/src/main.c
  git commit -m "style: reformat lesson-07 and translate comments to English"
  ```

---

## Task 8: Lesson 08 — Shadow and Highlight (Parts 1 & 2)

**Files:**
- `lesson-08-shadow-and-highlight-1/src/main.c` (224 lines)
- `lesson-08-shadow-and-highlight-2/src/main.c` (229 lines)

**Issues:** Portuguese accented characters in comments, Spanish comments, inverted/incorrect logic in display functions on lines 212-225 of part 2 (logic inversion — do NOT change logic, only formatting).

- [ ] **Step 1: Read both files**

  Read each `main.c`.

- [ ] **Step 2: Reformat part 1**

  Apply all rules:
  - Update header (`@brief Lesson 08 - "Shadow and Highlight (1)"`)
  - Spaces inside parentheses, braces everywhere
  - Replace Portuguese accented characters in comments with clean ASCII equivalents in English
  - Translate all comments

- [ ] **Step 3: Reformat part 2**

  Same rules. The apparent logic inversion on lines 212-225 is intentional in the original — do not change it, only reformat.

- [ ] **Step 4: Commit**

  ```bash
  git add lesson-08-shadow-and-highlight-1/src/main.c lesson-08-shadow-and-highlight-2/src/main.c
  git commit -m "style: reformat lesson-08 parts 1-2 and translate comments to English"
  ```

---

## Task 9: Lesson 08 — Shadow and Highlight (Parts 3, 4 & 5)

**Files:**
- `lesson-08-shadow-and-highlight-3-SF2/src/main.c` (133 lines)
- `lesson-08-shadow-and-highlight-4-SF2-maps/src/main.c` (183 lines)
- `lesson-08-shadow-and-highlight-5-Castlevania/src/main.c` (643 lines)

**Issues:** Spanish comments, mixed indentation on parts 4 (lines 15-16, 22-25), part 5 is the largest file in the project.

- [ ] **Step 1: Read all three files**

  Read each `main.c`.

- [ ] **Step 2: Reformat part 3 (SF2)**

  Apply all rules:
  - Update header (`@brief Lesson 08 - "Shadow and Highlight (3) — Street Fighter II"`)
  - Spaces inside parentheses, braces everywhere
  - Translate comments

- [ ] **Step 3: Reformat part 4 (SF2 maps)**

  Same rules. Fix the tab/space mix on lines 15-16 and 22-25.

- [ ] **Step 4: Reformat part 5 (Castlevania)**

  Same rules. Given its 643 lines, take extra care to:
  - Ensure blank lines separate each variable declaration block from the following logic
  - Ensure all nested structures are consistently indented at 4 spaces per level

- [ ] **Step 5: Commit**

  ```bash
  git add lesson-08-shadow-and-highlight-3-SF2/src/main.c lesson-08-shadow-and-highlight-4-SF2-maps/src/main.c lesson-08-shadow-and-highlight-5-Castlevania/src/main.c
  git commit -m "style: reformat lesson-08 parts 3-5 and translate comments to English"
  ```

---

## Task 10: Lesson 09 — Scroll (Parts 0, 1, 2 & 3)

**Files:**
- `lesson-09-scroll-0-by-planes/src/main.c` (353 lines)
- `lesson-09-scroll-1-by-tiles/src/main.c` (464 lines)
- `lesson-09-scroll-2-by-lines/src/main.c` (504 lines)
- `lesson-09-scroll-3-beyond-512px/src/main.c` (166 lines)

**Issues:** Spanish comments, UTF-8 issues (lines 18-19 of scroll-0 and scroll-1, line 19 of scroll-2), typo in title header of scroll-3.

- [ ] **Step 1: Read all four files**

  Read each `main.c`.

- [ ] **Step 2: Reformat scroll-0 (by planes)**

  Apply all rules:
  - Update header (`@brief Lesson 09 - "Scroll (0) — By Planes"`)
  - Fix corrupted characters on line 18
  - Spaces inside parentheses, braces everywhere
  - Translate comments

- [ ] **Step 3: Reformat scroll-1 (by tiles)**

  Same rules. Fix UTF-8 corruption on line 19.

- [ ] **Step 4: Reformat scroll-2 (by lines)**

  Same rules. Fix UTF-8 corruption on line 19.

- [ ] **Step 5: Reformat scroll-3 (beyond 512px)**

  Same rules. Fix the title header typo.

- [ ] **Step 6: Commit**

  ```bash
  git add lesson-09-scroll-0-by-planes/src/main.c lesson-09-scroll-1-by-tiles/src/main.c lesson-09-scroll-2-by-lines/src/main.c lesson-09-scroll-3-beyond-512px/src/main.c
  git commit -m "style: reformat lesson-09 files and translate comments to English"
  ```

---

## Task 11: Lesson 10 — Collisions

**Files:**
- `lesson-10-collisions-1/src/main.c` (549 lines)
- `lesson-10-collisions-2-bullet-hell-static-mem/src/main.c` (503 lines)
- `lesson-10-collisions-3-bullet-hell-dynamic-mem/src/main.c` (523 lines)
- `lesson-10-collisions-4-bullet-hell-dynamic-mem-optimized/src/main.c` (507 lines)

**Issues:** Spanish comments throughout large files, consistent code structure but missing spaces inside parentheses and braces on single-line ifs.

- [ ] **Step 1: Read all four files**

  Read each `main.c`.

- [ ] **Step 2: Reformat collisions-1**

  Apply all rules:
  - Update header (`@brief Lesson 10 - "Collisions (1)"`)
  - Spaces inside parentheses, braces on all control structures
  - Translate all Spanish comments

- [ ] **Step 3: Reformat collisions-2 (static mem)**

  Same rules. (`@brief Lesson 10 - "Collisions (2) — Bullet Hell (Static Memory)"`)

- [ ] **Step 4: Reformat collisions-3 (dynamic mem)**

  Same rules. (`@brief Lesson 10 - "Collisions (3) — Bullet Hell (Dynamic Memory)"`)

- [ ] **Step 5: Reformat collisions-4 (dynamic mem optimized)**

  Same rules. (`@brief Lesson 10 - "Collisions (4) — Bullet Hell (Dynamic Memory, Optimized)"`)

- [ ] **Step 6: Commit**

  ```bash
  git add lesson-10-collisions-1/src/main.c lesson-10-collisions-2-bullet-hell-static-mem/src/main.c lesson-10-collisions-3-bullet-hell-dynamic-mem/src/main.c lesson-10-collisions-4-bullet-hell-dynamic-mem-optimized/src/main.c
  git commit -m "style: reformat lesson-10 files and translate comments to English"
  ```

---

## Task 12: Lessons 11 & 13

**Files:**
- `lesson-11-interactive-scenarios/src/main.c` (596 lines)
- `lesson-13-the-window-plane/src/main.c` (419 lines)

**Issues:** Spanish comments throughout, lesson-13 already has some English comments mixed in (from the adaptation) — consolidate to English only.

- [ ] **Step 1: Read both files**

  Read each `main.c`.

- [ ] **Step 2: Reformat lesson-11**

  Apply all rules:
  - Update header (`@brief Lesson 11 - "Interactive Scenarios"`)
  - Spaces inside parentheses, braces on all control structures
  - Translate all Spanish comments

- [ ] **Step 3: Reformat lesson-13**

  Apply all rules:
  - Update header (`@brief Lesson 13 - "The Window Plane"`)
  - Remove any remaining Spanish comments (replace with English equivalents)
  - Ensure previously-added English comments follow the single-space `// comment` style (no double-space after `//`)
  - Spaces inside parentheses, braces on all control structures

- [ ] **Step 4: Commit**

  ```bash
  git add lesson-11-interactive-scenarios/src/main.c lesson-13-the-window-plane/src/main.c
  git commit -m "style: reformat lessons 11 and 13 and translate comments to English"
  ```

---

## Self-Review Checklist

- [x] All 29 files covered across Tasks 1-12
- [x] No TBD or placeholder steps — each file has explicit rules and, for smaller files, full expected output
- [x] Encoding corruption issue documented in Tasks 5, 9, and 10
- [x] Logic-inversion note in Task 8 part 2 — do not change logic, only format
- [x] Mixed tab/space indentation issue called out per file group
- [x] On-screen Spanish strings included in translation scope
- [x] Commit per task group for clean, reversible history
