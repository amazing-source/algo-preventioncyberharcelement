#ifndef STATS_H
#define STATS_H

#include "harassment_detector.h"

#define NUM_CATEGORIES 8

typedef struct {
    int total_comments;
    double average_score;
    int category_counts[NUM_CATEGORIES];
    int most_toxic_index;
    int most_toxic_score;
} Statistics;

void stats_init(Statistics *stats);
void stats_add_result(Statistics *stats, const AnalysisResult *result, int index);
void stats_print(const Statistics *stats);
const char* stats_get_category_name(int index);
int stats_get_category_index(const char *category);

#endif
