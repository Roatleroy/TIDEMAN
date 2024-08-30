#include <cs50.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }

            // printf("%i ", ranks[j]);
        }

        // printf("\n");
        record_preferences(ranks);
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    int j = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
        else
        {
            j++;
            if (j == candidate_count)
            {
                break;
            }
        }
    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]] += 1;
            // printf("Candidate %i is preferred over Candidate %i\n", ranks[i], ranks[j]);
        }
    }

    printf("\n");

    /*
             for (int i = 0; i < candidate_count; i++)
            {
                for (int j = 0; j < candidate_count; j++)
                {
                    printf("%i ", preferences[i][j]);
                }
                printf("\n");
            }

         printf("\n");

    */

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int t = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i; j < candidate_count; j++)
        {
            if (j != i)
            {
                if (preferences[i][j] > preferences[j][i])
                {
                    pairs[t].winner = i;
                    pairs[t].loser = j;
                    // printf("%s wins over %s, %i to %i\n", candidates[i], candidates[j],
                    // preferences[i][j], preferences[j][i]);
                    ++t;
                    pair_count++;
                }
                else if (preferences[i][j] < preferences[j][i])
                {
                    pairs[t].winner = j;
                    pairs[t].loser = i;
                    // printf("%s wins over %s, %i to %i\n", candidates[i], candidates[j],
                    // preferences[i][j], preferences[j][i]);
                    pair_count++;
                    ++t;
                }
            }
        }
    }

    /*

         for (int i = 0; i < pair_count; i++)
            {
                printf("winner %i, loser %i ", pairs[i].winner, pairs[i].loser);
                printf("\n");
            }

         printf("\n");

    */

    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{

    int keep = 0;
    int winS;
    int loseS;

    int array[pair_count];

    for (int i = 0; i < pair_count; ++i)
    {
        winS = preferences[pairs[i].winner][pairs[i].loser];
        loseS = preferences[pairs[i].loser][pairs[i].winner];
        winS = winS - loseS;
        array[i] = winS;
    }

    for (int i = 0; i < pair_count; ++i)
    {
        for (int j = 0; j < pair_count - i - 1; j++)
        {
            if (array[j] < array[j + 1])
            {
                keep = pairs[j].winner;
                pairs[j].winner = pairs[j + 1].winner;
                pairs[j + 1].winner = keep;

                keep = pairs[j].loser;
                pairs[j].loser = pairs[j + 1].loser;
                pairs[j + 1].loser = keep;

                winS = array[j];
                array[j] = array[j + 1];
                array[j + 1] = winS;
            }
        }
    }

    /*

            for (int i = 0, n = sizeof(array) / sizeof(array[0]); i < n; ++i)
            {
                printf("%i ", array[i]);
            }


            printf("\n \n");

            for (int i = 0; i < pair_count; i++)
            {
                printf("winner %i, loser %i ", pairs[i].winner, pairs[i].loser);
                printf("\n");
            }

            printf("\n \n");

    */

    return;
}

bool cycled(int loser, int winner);

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{

    for (int i = 0; i < pair_count; i++)
    {

        locked[pairs[i].winner][pairs[i].loser] = true;

        if (cycled(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
    }

    return;
}

bool cycled(int winner, int loser)
{
    if (winner == loser)
    {
        return true;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[i][winner])
        {
            if (cycled(i, loser))
            {
                return true;
            }
        }
    }

    return false;
}

// Print the winner of the election
void print_winner(void)
{

    int t = 0;

    // printf("\nPair Count: %i\n", pair_count);

    /*
        for (int i = 0; i < candidate_count; i++)
        {
            for (int j = 0; j < candidate_count; j++)
            {
                if (locked[i][j] == true)
                {
                    printf("true ");
                }
                else
                {
                    printf("false ");
                }
            }
            printf("\n");
        }

    */

    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == false)
            {
                t++;
            }
        }

        if (t == candidate_count)
        {
            printf("%s\n", candidates[i]);
        }

        t = 0;
    }

    // printf("\n");
    return;
}
