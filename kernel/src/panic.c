#include <panic.h>
#include <common.h>
#include <terminal.h>

extern void hang();

void panic(const char* error, const char* description, panic_handler_t handler) {
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

    // "Error: "
    terminal_set_colour(VGA_COLOUR(RED, BLACK));
    terminal_write("Error:       ");

    // show error
    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(error);

    // "Description: "
    terminal_set_colour(VGA_COLOUR(RED, BLACK));
    terminal_write("\nDescription: ");

    // show description
    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(description);

    // EAX
    terminal_write("\n\n        EAX: ");
    terminal_write_hex(eax);

    // EBX
    terminal_write(" EBX: ");
    terminal_write_hex(ebx);

    // ECX
    terminal_write(" ECX: ");
    terminal_write_hex(ecx);

    // EDX
    terminal_write(" EDX: ");
    terminal_write_hex(edx);

    // ESI
    terminal_write("\n        ESI: ");
    terminal_write_hex(esi);

    // EDI
    terminal_write(" EDI: ");
    terminal_write_hex(edi);

    // EBP
    terminal_write(" EBP: ");
    terminal_write_hex(ebp);
    
    // ESP
    terminal_write(" ESP: ");
    terminal_write_hex(esp);

    // call any additional handlers
    if (handler != NULL) {
        terminal_set_colour(VGA_COLOUR(RED, BLACK));
        terminal_write("\n\nAdditional information: ");
        terminal_set_colour(VGA_COLOUR(WHITE, BLACK));

        handler();
    }

    // hang
    hang();
}

void notify(const char* error, const char* description, panic_handler_t handler) {
    // save the current colour
    uint8_t old_colour = terminal_get_colour();

    // draw a box
    terminal_set_colour(VGA_COLOUR(LIGHT_BLUE, BLACK));
    terminal_box(0, 0, 80, 3);

    // "Kernel Notification: "
    terminal_set_colour(VGA_COLOUR(LIGHT_CYAN, BLACK));
    terminal_move(1, 1);
    terminal_write("Kernel Notification: ");

    // show notification
    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(error);

    // "Description: "
    terminal_set_colour(VGA_COLOUR(LIGHT_CYAN, BLACK));
    terminal_move(1, 2);
    terminal_write("Description: ");
    
    // show description
    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(description);

    // move outside of the box
    terminal_move(0, 5);

    // call any additional handlers
    if (handler != NULL) {
        terminal_set_colour(VGA_COLOUR(LIGHT_CYAN, BLACK));
        terminal_write("Additional information: ");
        terminal_set_colour(VGA_COLOUR(WHITE, BLACK));

        handler();
    }

    // return the terminal colour and skip a line.
    terminal_set_colour(old_colour);
    terminal_put('\n');
}