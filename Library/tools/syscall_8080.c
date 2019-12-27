/*
 *	Generate the syscall functions
 *
 *	8080 - Nice and simple as we use the compiler calling convention
 *	with an offset for the syscall stub
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscall_name.h"

static char namebuf[128];

static void write_call(int n)
{
  FILE *fp;
  snprintf(namebuf, 128, "fuzix8080/syscall_%s.s",syscall_name[n]);
  fp = fopen(namebuf, "w");
  if (fp == NULL) {
    perror(namebuf);
    exit(1);
  }
  fprintf(fp, "\t.sect .text\n\n");
  fprintf(fp, "\t.define _%s\n\n", syscall_name[n]);
  fprintf(fp, "_%s:\n\tmvi a,%d\n", syscall_name[n], n);
  fprintf(fp, "\tjmp __syscall\n");
  fclose(fp);
}

static void write_call_table(void)
{
  int i;
  for (i = 0; i < NR_SYSCALL; i++)
    write_call(i);
}

static void write_makefile(void)
{
  int i;
  char path[256];
  FILE *fp;

  fp = fopen("fuzix8080/Makefile", "w");
  if (fp == NULL) {
    perror("Makefile");
    exit(1);
  }
  fprintf(fp, "# Autogenerated by tools/syscall8080\n");
  fprintf(fp, "ASYS=syscall.s\n");
  fprintf(fp, "ASRCS = syscall_%s.s\n", syscall_name[0]);
  for (i = 1; i < NR_SYSCALL; i++)
    fprintf(fp, "ASRCS += syscall_%s.s\n", syscall_name[i]);
  fprintf(fp, "\n\nASRCALL = $(ASRCS) $(ASYS)\n");
  fprintf(fp, "\nAOBJS = $(ASRCALL:.s=.o)\n\n");
  fprintf(fp, "syslib.lib: $(AOBJS)\n");
  fprintf(fp, "\techo $(AOBJS) >syslib.l\n");
  fprintf(fp, "\taal rc syslib.lib $(AOBJS)\n\n");
  fprintf(fp, "$(AOBJS): %%.o: %%.s\n");
  fprintf(fp, "\tack -mfuzix -c -o $*.o $<\n\n");
  fprintf(fp, "clean:\n");
  fprintf(fp, "\trm -f $(AOBJS) $(ASRCS) syslib.lib *~\n\n");
  fclose(fp);
}

int main(int argc, char *argv[])
{
  write_makefile();
  write_call_table();
  exit(0);
}
