#include <keyboard.h>
#include <idt.h>

key_handler_t key_handlers[256];
uint16_t key_handler_amount = 0;

#define SHIFT 0xFF
#define CAPS 0xFE
#define CTRL 0xFD

uint8_t keymap[128] = {
    0,		27,		'1',	'2',	'3',	'4',	'5',	'6',	'7',	'8',
    '9',	'0',	'-',	'=',	'\b',	'\t',	'q',	'w',	'e',	'r',
    't',	'y',	'u',	'i',	'o',	'p',	'[',	']',	'\n',	CTRL,
    'a',	's',	'd',	'f',	'g',	'h',	'j',	'k',	'l',	';',
    '\'',	'`',  	SHIFT,	'#',	'z',	'x',	'c',	'v',	'b',	'n',
    'm',	',',	'.',	'/',	SHIFT,	'*',	0,  	' ',	CAPS,	0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		'-',	0,		0,		0,		'+',	0,
    0,		0,		0,		0,		0,		0,		'\\',	0,		0,	    0,
    0,		0,		0,		0,		0
};

uint8_t keymap_shift[128] = {
    0,		27,		'!',	'\"',	0x9C,  	'$',	'%',	'^',	'&',	'*',
    '(',	')',	'_',	'+',	'\b',	'\t',	'Q',	'W',	'E',	'R',
    'T',	'Y',	'U',	'I',	'O',	'P',	'{',	'}',	'\n',	CTRL,
    'A',	'S',	'D',	'F',	'G',	'H',	'J',	'K',	'L',	':',
    '@',	0xAA,  	SHIFT,	'~',	'Z',	'X',	'C',	'V',	'B',	'N',
    'M',	'<',	'>',	'?',	SHIFT,	'*',	0,		' ',	0,		0,
    0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		'-',	0,		0,		0,		'+',	0,
    0,		0,		0,		0,		0,		0,		'|',	0,		0,  	0,
    0,		0,		0,		0,		0
};

uint8_t keymap_caps[128] = {
    0,		27,		'1',	'2',	'3',	'4',	'5',	'6',	'7',	'8',
    '9',	'0',	'-',	'=',	'\b',	'\t',	'Q',	'W',	'E',	'R',
    'T',	'Y',	'U',	'I',	'O',	'P',	'[',	']',	'\n',	0,
    'A',	'S',	'D',	'F',	'G',	'H',	'J',	'K',	'L',	';',
    '\'',	'`',    SHIFT,	'#',	'Z',	'X',	'C',	'V',	'B',	'N',
    'M',	',',	'.',	'/',	SHIFT,	'*',	0,		' ',	CAPS,	0,
    0,		0,		0,		'\\',	0,		0,		0,		0,		0,		0,
    0,		0,		0,		0,		'-',	0,		0,		0,		'+',	0,
    0,		0,		0,		0,		0,		0,		'\\',	0,		0,  	0,
    0,		0,		0,		0,		0
};

bool shift_pressed = false;
bool caps_lock = false;
bool ctrl_pressed = false;

static uint8_t get_ascii(uint8_t scancode) {
    if (scancode & 0x80) { 
        if (keymap[scancode & 0x7F] == SHIFT) shift_pressed = false;
        else if (keymap[scancode & 0x7F] == CTRL) ctrl_pressed = false;
        return 0;
    }

    uint8_t ascii = shift_pressed ? keymap_shift[scancode] : (caps_lock ? keymap_caps[scancode] : keymap[scancode]);
    
    if (ascii == SHIFT) {
        shift_pressed = true;
    }
    
    else if (ascii == CTRL) {
        ctrl_pressed = true;
    }
    
    else if (ascii == CAPS) {
        caps_lock = !caps_lock;
    } 
    
    else if (ascii) {
        return ascii;
    }

    return 0;
}

static inline uint8_t get_modifiers() {
    return shift_pressed | (caps_lock << 1) | (ctrl_pressed << 2);
}

static void keyboard_callback(int_registers_t* regs) {
    uint8_t scancode = inb(0x60);

    // call all the registered handlers
    for (uint16_t i = 0; i < key_handler_amount; i++) {
        key_handlers[i](scancode, get_ascii(scancode), get_modifiers());
    }
}

void register_key_handler(key_handler_t handler) {
    key_handlers[key_handler_amount++] = handler;
}

void keyboard_init() {
    register_int_handler(IRQ1, keyboard_callback);
}