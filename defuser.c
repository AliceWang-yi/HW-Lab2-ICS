#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * �� values �е� count ������д��һ�С��ո�ָ����޷���ʮ���������������� out��
 * �ɹ����� 0��count Ϊ 0����û��ʵ�֣���snprintf �����������ض�ʱ���ط��㡣
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    uint32_t a = (uint32_t)challenge;
    uint32_t b = (uint32_t)(challenge >> 32);
    uint64_t c = challenge >> 16;
    uint32_t step0 = (b + 0x8b9d) ^ a;
    uint32_t step1 = (b + c * 4) ^ 0x7cc4;
    uint32_t answer = ((step1 * 5 + a) ^ 0xf35c) ^ step0;
    values[0] = answer;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;
    values[0] = ((challenge >> 32) & 0x3f) + 12;
    for (uint32_t i = 1; i < 6; i++) {
        uint32_t shift = 16 + i * 4;
        uint32_t v = ((challenge >> shift) & 0x3f) + i;
        v ^= values[i - 1];
        values[i] = v * 3;
    }

    return write_answer(out, cap, values, count);
}
int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    uint32_t index = (uint32_t)challenge & 7;
    uint32_t x = (uint32_t)((challenge >> 3) ^ (challenge >> 32));
    uint32_t answer = 0;
    switch (index) {
        case 0:
        {
            answer = x * 5 + 0x3cd93;
            break;
        }
        case 1:
        {
            answer = (x ^ 0x409f) + (x >> 1);
            break;
        }
        case 2:
        {
            answer = x * 5 + 0x3395a;
            break;
        }
        case 3:
        {
            answer = x * 5 + 0xb422;
            break;
        }
        case 4:
        {
            answer = x * 3 + 0xc7a4;
            break;
        }
        case 5:
        {
            answer = (x ^ 0x16c8) + (x >> 1);
            break;
        }
        case 6:
        {
            answer = (x * 4) ^ 0xd3f3;
            break;
        }
        case 7:
        {
            answer = (x * 4u) ^ 0x37c8u;
            break;
        }
    }

    values[0] = index;
    values[1] = answer;

    return write_answer(out, cap, values, count);
}
static uint32_t solve_p4_walk(uint32_t target,uint32_t low,uint32_t high,uint32_t key) {
    uint32_t mid = low + ((high - low) >> 1);

    if (mid == target) {return (mid + 0x43u) ^ key;}

    uint32_t mix = mid + key;

    if (target < mid) {
        uint32_t result =solve_p4_walk(target, low, mid - 1, key);
        return ((result * 5u) ^ mix) ^ 0x04u;
    }
    uint32_t result =solve_p4_walk(target, mid + 1, high, key);
    return ((result * 5u) ^ mix) ^ 0x1fu;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;

    uint32_t target = (uint32_t)challenge & 0x7fu;
    uint32_t key =(uint32_t)(challenge >> 7) ^(uint32_t)(challenge >> 32);

    values[0] = solve_p4_walk(target, 0, 0x7f, key);

    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;

    static const uint32_t node_values[11] = {
        273u, 1915u, 570u, 784u, 2661u, 1387u,
        2144u, 91u, 2348u, 1571u, 2910u
    };

    static const uint32_t node_weights[11] = {
        18063u, 59068u, 13961u, 51946u, 17775u, 50792u,
        14539u, 4275u, 48741u, 34307u, 24637u
    };

    uint32_t start = (uint32_t)challenge & 7u;
    uint32_t steps = ((uint32_t)(challenge >> 3) & 3u) + 1u;

    uint32_t hash =(uint32_t)(challenge >> 6) ^(uint32_t)(challenge >> 32) ^0x4067u;

    uint32_t position = start;

    for (uint32_t i = 0; i < steps; ++i) {
        position = start + i;
        hash = (hash + node_weights[position]) * 5u;
        hash ^= node_values[position];
    }

    values[0] = hash;
    values[1] = node_values[position];

    return write_answer(out, cap, values, count);
}


static uint32_t solve_p6_apply(uint32_t index, uint32_t x) {
    switch (index) {
        case 0:
        case 5:
            return ((x << 3) | (x >> 29)) ^ 0xaef9u;
        case 1:
        case 6:
            return ((x >> 2) ^ x) + 0x6ffau;
        case 2: {
            uint32_t four_x = x * 4u;
            return (four_x + 0x9e32u) ^ (four_x + x);
        }
        case 3:
            return (x >> 3) | (x << 29);
        case 4:
            return (x ^ 0x9d4cu) + (x << 7);
        case 7:
            return (x + 0x53fu) ^ (x >> 2);
    }
    return 0;
}
int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    uint32_t x =
        (uint32_t)(challenge >> 6) ^(uint32_t)(challenge >> 32);
    uint32_t first_index =
        (uint32_t)challenge & 7u;
    uint32_t second_index =
        ((uint32_t)challenge >> 3) & 7u;
    uint32_t answer = solve_p6_apply(first_index, x);
    answer ^= 0x1491u;
    answer = solve_p6_apply(second_index, answer);
    answer += 0xfff1u;
    values[0] = answer;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge,uint64_t context,char *out,size_t cap) {
    uint32_t selector = (uint32_t)context & 3u;
    uint32_t challenge_fold =(uint32_t)challenge ^(uint32_t)(challenge >> 32);
    uint32_t context_fold =(uint32_t)context ^(uint32_t)(context >> 32);
    uint32_t argument =challenge_fold ^ context_fold;
    uint32_t mixed =((argument >> 1) ^ 0xcfb2u) +(argument ^ 0x12fu);
    uint32_t second =mixed ^(selector * 0x12fu) ^(uint32_t)(context >> 16);
    uint32_t third =second ^(context_fold +selector +0xcfb2u +(challenge_fold >> 1));
    int written = snprintf(out,cap,
        "AAAAAAAAAAAAAAAA"
        "AAAAAAAAAAAAAAAA"
        "OPEN %" PRIu32 " %" PRIu32 " %" PRIu32,
        selector,
        second,
        third
    );
    if (written < 0 || (size_t)written >= cap) {
        return 1;
    }
    return 0;
}

