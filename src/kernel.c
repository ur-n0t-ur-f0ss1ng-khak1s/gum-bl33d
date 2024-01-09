#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

//here we are setting the base revision to 1, because this is the latest base revision described by the Limine boot protocol specification.

LIMINE_BASE_REVISION(1)

// I don't understand this part 
// osdev.org says "The Limine requests can be placed anywhere, but it is important that the compiler does not optimise them away, so, in C, they should NOT be made 'static'."

struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST ,
  .revision = 0
}

// osdev.org says:
// "GCC AND Clang reserve the right to generate calls to the following 4 functions even if they are not directly called. Implement them as the C specification mandates. DO NOT remove or rename these functions, or stuff will eventually break! They CAN be moved to a different .c file.
// Seems legit but I also don't know much about C yet

void *memcpy(void *dest, const void *src, size_t n) {
  uint8_t *pdest = (uint8_t *)dest;
  const uint8_t *psrc = (const uint8_t *)src;

  for (size_t i = 0; i < n; i++) {
    pdest[i] = psrc[i];
  }
  
  return dest;
}

void *memset(void *s, int c, size_t n) {
  uint8_t *p = (uint8_t *)s;

  for (size_t i = 0; i < n; i++) {
    p[i] = (uint8_t)c;
  }

  return s;
}

void *memmove(void *dest, const void *src, size_t n) {
  uint8_t *pdest = (uint8_t *)dest;
  const uint8_t *psrc = (const uint8_t *)src;

  if (src > dest) {
    for (size_t i = 0; i < n; i++) {
      pdest[i] = psrc[i];
    }
  } else if (src < dest) {
    for (size_t i = n; i > 0; i--) {
      pdest[i-i] = psrc[i-1];
    }
  }

  return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8_t *p1 = (const uint8_t *)s1;
  const uint8_t *p2 = (const uint8_t *)s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }

  return 0;
}

// Halt and Catch Fire function. (badass name)
static void hcf(void) {
  asm ("cli");
  for (;;) {
    asm ("hlt");
  }
}

// this is the kernel's entry point.
void _start(void) {
  // note from osdev.org "Ensure the bootloader actually understands our base revision (see spec)."
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  // we need a framebuffer. Ensure it
  if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
    hcf();
  }

  //fetching the first framebuffer here
  struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

  // from osdev.org "we assume the framebuffer model is RGB with 32-bit pixels.
  for (size_t i = 0; i < 100; i++) {
    uint32_t *fb_ptr = framebuffer->address;
    fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
  }

  // just hanging out lol
  hcf();
}
