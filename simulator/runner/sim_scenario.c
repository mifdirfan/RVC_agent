/**
 * @file sim_scenario.c
 * @brief Minimal parser for the fixed scenario JSON schema (sim_scenario.h).
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sim_scenario.h"

static const char *skip_ws(const char *p)
{
    while (*p != '\0' && isspace((unsigned char)*p)) {
        ++p;
    }
    return p;
}

/* Finds "key" followed by ':' inside [begin, end). Returns the char after ':'. */
static const char *find_key(const char *begin, const char *end, const char *key)
{
    char pat[64];
    int n = snprintf(pat, sizeof pat, "\"%s\"", key);
    if (n <= 0 || (size_t)n >= sizeof pat) {
        return NULL;
    }
    const char *p = begin;
    while ((p = strstr(p, pat)) != NULL && p < end) {
        const char *q = skip_ws(p + n);
        if (*q == ':') {
            return skip_ws(q + 1);
        }
        p += n;
    }
    return NULL;
}

/* End of the {...} or [...] that starts at p (matching bracket), or NULL. */
static const char *match_bracket(const char *p)
{
    char open = *p;
    char close = (open == '{') ? '}' : ']';
    int depth = 0;
    bool in_str = false;
    for (; *p != '\0'; ++p) {
        if (in_str) {
            if (*p == '\\' && p[1] != '\0') {
                ++p;
            } else if (*p == '"') {
                in_str = false;
            }
        } else if (*p == '"') {
            in_str = true;
        } else if (*p == open) {
            ++depth;
        } else if (*p == close) {
            if (--depth == 0) {
                return p;
            }
        }
    }
    return NULL;
}

static bool parse_int(const char *p, int *out)
{
    char *endp;
    long v;
    if (p == NULL) {
        return false;
    }
    v = strtol(p, &endp, 10);
    if (endp == p) {
        return false;
    }
    *out = (int)v;
    return true;
}

typedef enum { CELL_OBSTACLE, CELL_DUST } CellKind;

/* Parses [[x,y],...] at p into the map. */
static bool parse_pairs(const char *p, Scenario *sc, CellKind kind, const char *name,
                        char *err, size_t err_len)
{
    if (p == NULL) {
        return true; /* key absent = empty list */
    }
    if (*p != '[') {
        snprintf(err, err_len, "\"%s\" must be an array of [x, y] pairs", name);
        return false;
    }
    const char *end = match_bracket(p);
    if (end == NULL) {
        snprintf(err, err_len, "\"%s\": unterminated array", name);
        return false;
    }
    const char *q = skip_ws(p + 1);
    while (q < end) {
        if (*q == ',') {
            q = skip_ws(q + 1);
            continue;
        }
        if (*q != '[') {
            snprintf(err, err_len, "\"%s\": expected [x, y]", name);
            return false;
        }
        char *e1;
        char *e2;
        long x = strtol(q + 1, &e1, 10);
        const char *c = skip_ws(e1);
        if (e1 == q + 1 || *c != ',') {
            snprintf(err, err_len, "\"%s\": malformed pair", name);
            return false;
        }
        const char *y_start = c + 1;
        long y = strtol(y_start, &e2, 10);
        c = skip_ws(e2);
        if (e2 == y_start || *c != ']') {
            snprintf(err, err_len, "\"%s\": malformed pair", name);
            return false;
        }
        if (x < 0 || x >= GRID_W || y < 0 || y >= GRID_H) {
            snprintf(err, err_len, "%s at (%ld, %ld) is outside the 10x10 grid", name, x, y);
            return false;
        }
        bool *cell = (kind == CELL_OBSTACLE) ? &sc->map.obstacle[y][x] : &sc->map.dust[y][x];
        if (*cell) {
            snprintf(err, err_len, "duplicate %s at (%ld, %ld)", name, x, y);
            return false;
        }
        *cell = true;
        q = skip_ws(c + 1);
    }
    return true;
}

bool scenario_parse(const char *json, Scenario *out, char *err, size_t err_len)
{
    if (json == NULL || out == NULL || err == NULL || err_len == 0) {
        return false;
    }
    memset(out, 0, sizeof *out);
    err[0] = '\0';
    const char *json_end = json + strlen(json);

    /* start */
    const char *st = find_key(json, json_end, "start");
    if (st == NULL || *st != '{') {
        snprintf(err, err_len, "missing \"start\": {\"x\", \"y\", \"dir\"}");
        return false;
    }
    const char *st_end = match_bracket(st);
    if (st_end == NULL) {
        snprintf(err, err_len, "\"start\": unterminated object");
        return false;
    }
    if (!parse_int(find_key(st, st_end, "x"), &out->start.x) ||
        !parse_int(find_key(st, st_end, "y"), &out->start.y)) {
        snprintf(err, err_len, "\"start\" needs integer \"x\" and \"y\"");
        return false;
    }
    const char *d = find_key(st, st_end, "dir");
    char dir[4] = { 0 };
    if (d == NULL || *d != '"' || d[1] == '\0' || d[2] != '"') {
        snprintf(err, err_len, "\"start.dir\" must be one of \"N\", \"E\", \"S\", \"W\"");
        return false;
    }
    dir[0] = d[1];
    if (!heading_parse(dir, &out->start.dir)) {
        snprintf(err, err_len, "\"start.dir\" must be one of \"N\", \"E\", \"S\", \"W\"");
        return false;
    }
    if (out->start.x < 0 || out->start.x >= GRID_W || out->start.y < 0 || out->start.y >= GRID_H) {
        snprintf(err, err_len, "start (%d, %d) is outside the 10x10 grid", out->start.x, out->start.y);
        return false;
    }

    /* max_ticks */
    const char *mt = find_key(json, json_end, "max_ticks");
    out->max_ticks = SIM_TICK_LIMIT;
    if (mt != NULL && !parse_int(mt, &out->max_ticks)) {
        snprintf(err, err_len, "\"max_ticks\" must be an integer");
        return false;
    }
    if (out->max_ticks < 1 || out->max_ticks > SIM_TICK_LIMIT) {
        snprintf(err, err_len, "\"max_ticks\" must be 1..%d (got %d)", SIM_TICK_LIMIT, out->max_ticks);
        return false;
    }

    /* cells */
    if (!parse_pairs(find_key(json, json_end, "obstacles"), out, CELL_OBSTACLE, "obstacle", err, err_len) ||
        !parse_pairs(find_key(json, json_end, "dust"), out, CELL_DUST, "dust", err, err_len)) {
        return false;
    }

    /* cross checks */
    if (out->map.obstacle[out->start.y][out->start.x]) {
        snprintf(err, err_len, "start (%d, %d) is on an obstacle", out->start.x, out->start.y);
        return false;
    }
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            if (out->map.obstacle[y][x] && out->map.dust[y][x]) {
                snprintf(err, err_len, "cell (%d, %d) is both obstacle and dust", x, y);
                return false;
            }
        }
    }
    return true;
}
