#include <panic.h>
#include <common.h>
#include <terminal.h>

extern void hang();

void panic(const char* error, const char* description, panic_handler_t handler) {
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp;
    
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

    terminal_clear();

    terminal_set_colour(VGA_COLOUR(RED, BLACK));
    terminal_write("Error:       ");

    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(error);

    terminal_set_colour(VGA_COLOUR(RED, BLACK));
    terminal_write("\nDescription: ");

    terminal_set_colour(VGA_COLOUR(WHITE, BLACK));
    terminal_write(description);

    terminal_write("\n\n        EAX: ");
    terminal_write_hex(eax);

    terminal_write(" EBX: ");
    terminal_write_hex(ebx);

    terminal_write(" ECX: ");
    terminal_write_hex(ecx);

    terminal_write(" EDX: ");
    terminal_write_hex(edx);

    terminal_write("");

    terminal_write("\n        ESI: ");
    terminal_write_hex(esi);

    terminal_write(" EDI: ");
    terminal_write_hex(edi);

    terminal_write(" EBP: ");
    terminal_write_hex(ebp);
    
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