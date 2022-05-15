#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
using namespace std;

int itemCount;
int capacity;
int *profits, *weights;
int bestProfit;

void read_ksack(char *infile)
{
    FILE *fp;
    int i;

    printf("Input file: %s \n", infile);
    fp = fopen(infile, "r");
    if (fp == NULL)
    {
        printf("File not opened!\n");
        exit(1);
    }
    fscanf(fp, "%d %d", &itemCount, &capacity);

    profits = (int *)malloc(itemCount * sizeof(int));
    weights = (int *)malloc(itemCount * sizeof(int));

    for (i = 0; i < itemCount; i++)

        fscanf(fp, "%d %d", &profits[i], &weights[i]);
    fclose(fp);
}
int **initialize(int popSize, int chrLength)
{

    int **individuals = 0;
    individuals = new int *[popSize];
    for (int i = 0; i < popSize; i++)
    {
        individuals[i] = new int[chrLength];
        for (int j = 0; j < chrLength; j++)
        {
            int g = rand() % 2;
            individuals[i][j] = g;
        }
    }

    return individuals;
}

int *calculateFitness(int popSize, int itemcount, int **individuals)
{
    int *individualsFitness = 0;
    individualsFitness = new int[popSize];
    int totalWeight = 0;
    int totalValue = 0;
    for (int i = 0; i < popSize; i++)
    {
        for (int j = 0; j < itemcount; j++)
        {
            if (individuals[i][j] == 1)
            {
                totalWeight += weights[j];
                totalValue += profits[j];
            }
        }
        if (totalWeight <= capacity)
        {
            individualsFitness[i] = totalValue;
        }
        else
        {
            individualsFitness[i] = 1;
        }
        totalWeight = 0;
        totalValue = 0;
    }
    return individualsFitness;
}
int totalFitnessFunction(int popSize, int *individualsFitness)
{
    int totalFitness = 0;
    for (int i = 0; i < popSize; i++)
    {
        totalFitness += individualsFitness[i];
    }
    return totalFitness;
}
float *calculateProbabilities(int popSize, int *individualsFitness, int totalFitness)
{

    float *probs = 0;
    probs = new float[popSize];
    for (int i = 0; i < popSize; i++)
    {
        probs[i] = float(individualsFitness[i]) / float(totalFitness);
    }
    return probs;
}
float *calculateCumProbabilities(int popSize, float *probabilities)
{

    float sumCum = 0;
    float *cumProb = 0;
    cumProb = new float[popSize];
    for (int i = 0; i < popSize; i++)
    {
        sumCum += probabilities[i];
        cumProb[i] = sumCum;
    }
    return cumProb;
}
int **matePollFunction(int popSize, int chrLength, int **individuals, float *cumProb)
{

    int selection = 0;

    int **matePoll = 0;
    matePoll = new int *[popSize];

    for (int i = 0; i < popSize; i++)
    {
        matePoll[i] = new int[chrLength];
        double r = ((double)rand() / (RAND_MAX));
        for (int j = 0; j < popSize; j++)
        {

            if (r <= cumProb[j])
            {

                selection = j;

                break;
            }
        }
        for (int k = 0; k < chrLength; k++)
        {
            matePoll[i][k] = individuals[selection][k];
        }
    }
    return matePoll;
}
int **crossOver(int popSize, int chrLength, int **matedPoll, float probCrosOver)
{

    int **crosedPool = 0;
    crosedPool = new int *[popSize];
    for (int i = 0; i < popSize; i += 2)
    {
        crosedPool[i] = new int[chrLength];
        crosedPool[i + 1] = new int[chrLength];
        int crosOverSite = 1 + rand() % (chrLength - 2);
        double rForOnePoint = ((double)rand() / (RAND_MAX));
        if (rForOnePoint < probCrosOver)
        {
            for (int j = 0; j < crosOverSite; j++)
            {

                crosedPool[i][j] = matedPoll[i][j];
                crosedPool[i + 1][j] = matedPoll[i + 1][j];
            }
            for (int j = crosOverSite; j < chrLength; j++)
            {

                crosedPool[i][j] = matedPoll[i + 1][j];
                crosedPool[i + 1][j] = matedPoll[i][j];
            }
        }
        else
        {

            for (int j = 0; j < chrLength; j++)
            {

                crosedPool[i][j] = matedPoll[i][j];
                crosedPool[i + 1][j] = matedPoll[i + 1][j];
            }
        }
    }
    return crosedPool;
}

int **mutation(int popSize, int chrLength, int **crosedPool, float probOfMutation)
{

    int **mutationPool = 0;
    mutationPool = new int *[popSize];
    for (int i = 0; i < popSize; i++)
    {
        mutationPool[i] = new int[chrLength];
        for (int j = 0; j < chrLength; j++)
        {
            double randomMutation = (double)rand() / ((double)RAND_MAX + 1);

            if (randomMutation < probOfMutation)
            {
                if (crosedPool[i][j] == 0)
                {
                    mutationPool[i][j] = 1;
                }
                else
                {
                    mutationPool[i][j] = 0;
                }
            }
            else
            {
                mutationPool[i][j] = crosedPool[i][j];
            }
        }
    }
    return mutationPool;
}

int main()
{
    srand(time(NULL));
    int popSize, maxGenerations, chrLength;
    float probCrosOver, probOfMutation;
    int generations = 0;

    cout << "Type Pop Size: ";
    cin >> popSize;
    cout << "Type Max Generations: ";
    cin >> maxGenerations;
    cout << "Probabilit of Crossover: ";
    cin >> probCrosOver;
    cout << "Probabilit of Mutation: ";
    cin >> probOfMutation;
    cout << "***********************************************";
    cout << "\n";
    read_ksack("data.txt");
    int **individuals = initialize(popSize, itemCount);
    int **bestIndividual = 0;
    bestIndividual = new int *[1];
    int bestFitnessCount = 0;
    while (generations < maxGenerations)
    {

        int *individualsFitness = calculateFitness(popSize, itemCount, individuals);
        int totalFitness = totalFitnessFunction(popSize, individualsFitness);
        float *probabilities = calculateProbabilities(popSize, individualsFitness, totalFitness);
        float *cumprobabilities = calculateCumProbabilities(popSize, probabilities);
        int **matePoll = matePollFunction(popSize, itemCount, individuals, cumprobabilities);
        int **crosedPool = crossOver(popSize, itemCount, matePoll, probCrosOver);
        int **mutatedPool = mutation(popSize, itemCount, crosedPool, probOfMutation);

        for (int i = 0; i < popSize; i++)
        {

            for (int j = 0; j < itemCount; j++)
            {
                individuals[i][j] = mutatedPool[i][j];
            }
        }
        int *individualsFitnessU = calculateFitness(popSize, itemCount, individuals);
        for (int i = 0; i < popSize; i++)
        {

            if (individualsFitnessU[i] > bestFitnessCount)
            {
                bestIndividual[0] = new int[itemCount];
                cout << "Best Individual Temporary: ";
                for (int j = 0; j < itemCount; j++)
                {
                    bestIndividual[0][j] = individuals[i][j];
                    cout << bestIndividual[0][j];
                }
                bestFitnessCount = individualsFitnessU[i];
                cout << " --> Fitness Value: " << bestFitnessCount;
                cout << "\n";
            }
        }

        generations++;
    }

    for (int i = 0; i < 1; i++)
    {
        cout << "Best Individual Final"
             << " : ";
        for (int j = 0; j < itemCount; j++)
        {
            cout << bestIndividual[i][j];
        }

        cout << "\n";
    }
    cout << "Final Value : ";
    cout << bestFitnessCount;
    return 0;
}
