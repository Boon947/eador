/*
 * Copyright 2015 Boon
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
#include <stdlib.h>	// exit
#include <string.h>	// strchr
#include <setjmp.h>	// setjmp, longjmp
#include <stdio.h>	// getc, printf
#include "map.h"

// PARSE CONTEXT
static FILE *fp;
static jmp_buf env;
static char buf[800];
static int idx, tail, more, all;

static int mygets(char *s, int size, FILE *fp)
{
  int c, n = 0;
  while ((c = getc(fp)) != EOF && (s[n++] = c) != '\n' && n < size-1);
  s[n] = 0;
  return n-1;
}

static char *wrap_gets(void)
{
  for (;;) {
    int n = mygets(buf, sizeof buf, fp);
    if (n > 0) {
      char *s = &buf[n-1];
      if (*s == ';') more = 0; else more = 1;
      s[more] = 0;
      return strchr(buf, ':');
    }
    if (n < 0) longjmp(env, 42);
    tail = 0;
  }
}

static void print_item(char *key, char *idx, hash_t seed)
{
  char *name = NULL;
  if (key) name = map_lookup(key, seed);
  if (name) printf("<span title=\"%s\">", name);
  printf("%s", idx);
  if (name) printf("</span>");
}

static void parse_complex_list(char *t)
{
  hash_t seed = fnv_hash(buf, t-buf, FNV_SEED);
  do {
    char *s = wrap_gets();
    if (s == NULL) continue;
    putchar(tail ? ',' : ' ');
    print_item(all || !tail ? s : NULL, s+2, seed);
    tail = 1;
  } while (more);
}

static void aux_parse_list(char *s, char **tab)
{
  int n = 0, sync;
  for (sync = 1; *s != 0; ++s)
  {
    int c = *s;
    if ('0' <= c && c <= '9') { if (sync) tab[n++] = s, sync = 0; } else
    if (c == ',' || c == ';') *s = 0, sync = 1; else
    if (c == '(' || c == ')') *s = 0;
  }
  tab[n] = 0;
}

static void parse_list(char *t)
{
  int pair = 0, len = t-buf;
  char **idx, *argv[20];
  if (strstr(buf, "upgrades:")) pair = 1, len = sprintf(buf, "%s: ", "UnitAttr");
  if (strstr(buf,     "loot:")) pair = 1, len = sprintf(buf, "%s: ", "Items");
  hash_t seed = fnv_hash(buf, len, FNV_SEED);
  aux_parse_list(t, argv);
  for (idx = argv; *idx != NULL; ++idx) {
    print_item(*idx, *idx, seed);
    if (pair) printf(",%s", *(++idx));
    putchar(' ');
  }
}

static void parse_item_set(char *t)
{
  int n = atoi(t+2);
  do {
    char *s = wrap_gets();
    printf("%s:", s+2);
    int kk = sprintf(buf, "%s", "Effects");
    parse_complex_list(buf+kk);
    if (n > 1) printf(" \x85 ");
  } while (--n > 0);
}

static void parse_record(void)
{
  printf("\n<tr><td>%d", ++idx);
  while ( 1 ) {
    char *s = wrap_gets();
    if (buf[0] == '/') break;
    if (s == NULL) continue;
    printf("<td>");
    if (!strncmp(buf, "Bonus:", 6)) parse_item_set(s); else
    if (!strncmp(buf, "Items:", 6)) parse_list(s+2); else
    if (s[1] == '\0') parse_complex_list(s); else
    if (s[2] == '(' ) parse_list(s+2); else
    print_item(buf, s+2, FNV_SEED);
  }
}

static void parse_header(void)
{
  while ( 1 ) {
    char *s = wrap_gets();
    if (buf[0] == '/') break;
    if (s == NULL) continue;
    *s = 0;
    printf("<th>");
    char *h = buf;
    while (*h != 0) printf("%c<br>", *h++);
    if (s[1] == 0) do wrap_gets(); while (more);
  }
}

static void parse_desc(char *filename, char *prefix)
{
  int n, i = 0;
  FILE *fp_desc = fopen(filename, "r");
  if (fp_desc == NULL) return;

  while ((n = mygets(buf, sizeof buf, fp_desc)) >= 0)
  {
    if (!strncmp(buf, "Name:", 5)) {
      char key[40]; buf[n-1] = 0;
      sprintf(key, "%s: %d", prefix, i++);
      // Ignore "Пусто" and "Empty"
      static const char Pusto[] = "\xcf\xf3\xf1\xf2\xee";
      if (!strcmp(buf+6, Pusto) || !strcmp(buf+6, "Empty")) continue;
      map_insert(key, buf+6, n-6);
    }
  }
  fclose(fp_desc);
}

static const char HEADER[] =
  "<!doctype html>\n<title>%s</title>\n"
  "<meta charset=windows-1251>\n<script src=../table.js></script>\n"
  "<table id=foo class=filterable>\n<tr><th>";

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: to_html file.var\n");
    return 1;
  }
  fp = fopen(argv[1], "r");
  if (fp == NULL) return 2;

  parse_desc(  "item_slot.var", "Slot");
  parse_desc(  "item_type.var", "Class");
  parse_desc(  "item.var", "Items");
  parse_desc("inner_build.var", "Building");
  parse_desc("resource.var", "Resource");
  parse_desc("resource.var", "ResourceBonus");
  //parse_desc("build_group.var", "Group");
  //parse_desc("inner_build.doc", "INNER");
  //parse_desc("outer_build.doc", "OUTER");
  parse_desc("abilities.doc", "Effects");
  parse_desc("abilities.doc", "AntiEffect");
  parse_desc("unit_upg.var", "UnitAttr");
  parse_desc("unit_type.var", "UnitKind");
  parse_desc("unit_class.var", "UnitClass");
  parse_desc("unit.var", "Units");
  parse_desc("guard_type.var", "Guards");
  parse_desc("ritual.doc", "rEffects");
  parse_desc("defender.doc", "dAbility");
  parse_desc("site.doc", "sAbility");

  all = !strcmp(argv[1], "unit.var");

  int c;
  while ((c = getc(fp)) != '/' );
  while ((c = getc(fp)) != '\n');
  printf(HEADER, argv[1]);
  parse_header();
  if (setjmp(env) == 0) for (;;) parse_record();
  printf("\n</table>\n");
  fclose(fp);
  return 0;
}
