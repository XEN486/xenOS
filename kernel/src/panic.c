#include <panic.h>
#include <terminal.h>
#include <string.h>

void panic(const char* error, const char* description, panic_handler_t handler, const char* file, int line) {
#ifndef NO_PANIC
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp;
    
    // read all the current registers
    __asm__ volatile (
        "movl %%eax, %0\n"
        "movl %%ebx, %1\n"
        "movl %%ecx, %2\n"
        "movl %%edx, %3\n"
        "movl %%esi, %4\n"
        "movl %%edi, %5\n"
        "movl %%ebp, %6\n"
        "movl %%esp, %7\n"
        : "=g" (eax), "=g" (ebx), "=g" (ecx), "=g" (edx), 
          "=g" (esi), "=g" (edi), "=g" (ebp), "=g" (esp)
    );

    // clear the screen
    terminal_clear();

    // "Error: ".
    terminal_set_colour(VGA_COLOUR(RED, BLACK));
    terminal_write("Error:       ");

    // show error.
    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(error);

    // "Description: ".
    terminal_set_colour(VGA_COLOUR(RED, BLACK));
    terminal_write("\nDescription: ");

    // show description.
    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(description);

    // "Where: ".
    terminal_set_colour(VGA_COLOUR(RED, BLACK));
    terminal_write("\nWhere:       ");

    // file(line).
    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(file);
    terminal_write("(");
    terminal_write_dec(line);
    terminal_write(").");

    // EAX.
    terminal_write("\n\n        EAX: ");
    terminal_write_hex(eax);

    // EBX.
    terminal_write(" EBX: ");
    terminal_write_hex(ebx);

    // ECX.
    terminal_write(" ECX: ");
    terminal_write_hex(ecx);

    // EDX.
    terminal_write(" EDX: ");
    terminal_write_hex(edx);

    // ESI.
    terminal_write("\n        ESI: ");
    terminal_write_hex(esi);

    // EDI.
    terminal_write(" EDI: ");
    terminal_write_hex(edi);

    // EBP.
    terminal_write(" EBP: ");
    terminal_write_hex(ebp);
    
    // ESP.
    terminal_write(" ESP: ");
    terminal_write_hex(esp);

    // call any additional handlers.
    if (handler != NULL) {
        terminal_set_colour(VGA_COLOUR(RED, BLACK));
        terminal_write("\n\nAdditional information: ");
        terminal_set_colour(VGA_COLOUR(WHITE, BLACK));

        handler();
    }

    // hang.
    hang(true);
#endif
}

void notify(const char* error, char* description, panic_handler_t handler) {
#ifndef NO_NOTIFY
    // truncate the description if necessary.
    bool truncated = false;
    if (strlen(description) > TERM_WIDTH - 16) {
        description[TERM_WIDTH - 16] = '\0';
        truncated = true;
    }

    // save the current colour.
    uint8_t old_colour = terminal_get_colour();

    // draw a box.
    terminal_set_colour(VGA_COLOUR(LIGHT_BLUE, BLACK));
    terminal_box(0, 0, TERM_WIDTH, 2);

    // "Kernel Notification: ".
    terminal_set_colour(VGA_COLOUR(LIGHT_CYAN, BLACK));
    terminal_move(1, 1);
    terminal_write("Kernel Notification: ");

    // write notification.
    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(error);

    // "Description: ".
    terminal_set_colour(VGA_COLOUR(LIGHT_CYAN, BLACK));
    terminal_move(1, 2);
    terminal_write("Description: ");
    
    // write description.
    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(description);

    // move outside of the box.
    terminal_move(0, 4);

    // call any additional handlers.
    if (handler != NULL) {
        terminal_set_colour(VGA_COLOUR(LIGHT_CYAN, BLACK));
        terminal_write("Additional information: ");
        terminal_set_colour(VGA_COLOUR(WHITE, BLACK));

        handler();
        terminal_put('\n');
    }

#ifdef VERBOSE_DEBUG
    if (truncated) {
        terminal_write("Notification description has been truncated!\n");
    }
#else
    (void)truncated;
#endif

    // return the terminal colour.
    terminal_set_colour(old_colour);
#endif
}

void assert(bool condition, const char* error, char* description) {
    if (!condition) {
#ifndef NO_NOTIFY
        notify(error, description, NULL);
#elif defined(ASSERT_PANIC)
        PANIC(error, description, NULL);
#endif
    }
}