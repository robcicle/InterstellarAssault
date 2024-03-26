#include "ScoreSystem.h"
#include "Game.h"

#include <cassert>    // for assert()
#include <sstream>    // for std::stringstream
#include <fstream>    // for std::ofstream
#include <algorithm>  // for std::sort


// Constructor: Initializes the current score and loads the existing scores from the file
ScoreSystem::ScoreSystem()
    : mCurrScore(0, 0, "NULL") // Initializing current score
{
    // Load in the scores.dat file, if it exists
    LoadScores(); // Load scores from the file

    // If we have loaded scores, sort them
    if (mParsedScores.size() > 0)
        SortScoresVec();
}

// Destructor: Saves the scores to the file when the object is destroyed
ScoreSystem::~ScoreSystem()
{
    // Save the score vector
    SaveScores(); // Save current scores into the file
}

// function to add points to the current score
void ScoreSystem::AddCurrentPoints(int _points)
{
    mCurrScore.points += _points; // Add points to current score
}

// function to set the name for the current score
void ScoreSystem::AddCurrentName(std::string _name)
{
    mCurrScore.name = _name; // Set name for current score
}

// function to clear the current score and add it to the vector of parsed scores
void ScoreSystem::ClearCurrentScore()
{
    // Add current score to the list and reset it
    if(mCurrScore.points > 0)
        mParsedScores.push_back(Score(mParsedScores.size(), mCurrScore.points, mCurrScore.name));
    SortScoresVec(); // Sort the scores after adding the new one

    mCurrScore = Score(0, 0, "NULL"); // Reset current score
}

// function to retrieve the highest score from the list of parsed scores
int ScoreSystem::GetHighestScore() const
{
    if (!mParsedScores.empty())
        return mParsedScores[0].points; // Return the highest score
    return 0; // If no scores, return 0
}

// function to save the scores to the file
void ScoreSystem::SaveScores()
{
    // Create an integer for our for loop
    int parsedScoresSize = mParsedScores.size();

    // No scores to save
    if (parsedScoresSize <= 0)
        return;
    else if (parsedScoresSize > GC::MAX_SCORES_SAVE)
        parsedScoresSize = GC::MAX_SCORES_SAVE; // Limit the number of saved scores

    // Ensure scores are sorted before saving
    SortScoresVec();

    // Vector to hold scores that will be saved
    std::vector<Score> scoresToSave;
    for (int i = 0; i < parsedScoresSize; i++)
    {
        mParsedScores[i].id = i; // Reassign the ids for consistency
        scoresToSave.push_back(mParsedScores[i]); // Add to the save list
    }

    mParsedScores = scoresToSave; // Replace current scores with the ones to save

    // Serialize and encrypt the scores before saving
    std::string serialized = SerializeScores();
    std::string encrypted = EncryptDecrypt(serialized);

    // Write the encrypted scores to the file
    std::ofstream file(GC::SCORE_FILE_PATH, std::ios::out | std::ios::binary);
    if (file)
    {
        file << encrypted; // Write encrypted data to file
        file.close();
    }
    else
        assert(file); // Ensure the file is openable, else assert
}

// function to load the scores from the file
void ScoreSystem::LoadScores()
{
    // Read the encrypted data from the file
    std::ifstream file(GC::SCORE_FILE_PATH, std::ios::in | std::ios::binary);
    std::string encrypted((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Decrypt and deserialize the scores
    std::string decrypted = EncryptDecrypt(encrypted);
    DeserializeScores(decrypted);
}

// function to encrypt or decrypt the data
std::string ScoreSystem::EncryptDecrypt(const std::string& data)
{
    std::string result = data;

    // Simple XOR encryption/decryption
    for (size_t i = 0; i < data.size(); i++)
        result[i] = data[i] ^ GC::ENCRYPT_KEY[i % GC::ENCRYPT_KEY.size()];

    return result;
}

// function to serialize scores into a string
std::string ScoreSystem::SerializeScores()
{
    std::stringstream ss;
    for (const auto& score : mParsedScores)
        ss << score.id << ',' << score.points << ',' << score.name << ';';

    return ss.str(); // Return the serialized string
}

// function to deserialize scores from a string
void ScoreSystem::DeserializeScores(const std::string& data)
{
    std::stringstream ss(data);
    std::string segment;
    mParsedScores.clear();

    // Parse each score segment and add it to the vector
    while (std::getline(ss, segment, ';'))
    {
        std::stringstream segmentStream(segment);
        std::string part;
        int id, points;
        std::string name;

        // Extract id, points, and name
        std::getline(segmentStream, part, ',');
        id = std::stoi(part);

        std::getline(segmentStream, part, ',');
        points = std::stoi(part);

        std::getline(segmentStream, part, ',');
        name = part;

        mParsedScores.emplace_back(id, points, name); // Add to parsed scores
    }
}

// function to sort the vector of scores
void ScoreSystem::SortScoresVec()
{
    // Sort the vectors by points Highest = first, Lowest = last
    std::sort(mParsedScores.begin(), mParsedScores.end(), [](const Score& lhs, const Score& rhs) {
        return lhs.points > rhs.points; // Sorting in descending order
        });
}