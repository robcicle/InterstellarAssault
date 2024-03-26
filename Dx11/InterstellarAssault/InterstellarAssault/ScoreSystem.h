#pragma once

#include <string>  // for std::vector
#include <vector>  // for std::string


// ScoreSystem class to manage game scores
class ScoreSystem
{
public:
	// Nested Score struct to handle individual scores
	struct Score
	{
	public:
		Score(int _id, int _points, std::string _name)
			: id(_id), points(_points), name(_name)
		{}

		int id = 0;                 // Unique identifier for the score
		int points = 0;             // Points for the score
		std::string name = "NONE";  // Name associated with the score
	};

public:
	ScoreSystem();   // Constructor
	~ScoreSystem();  // Destructor

	void AddCurrentPoints(int _points);      // Add points to the current score
	void AddCurrentName(std::string _name);  // Set the name for the current score

	void ClearCurrentScore();  // Reset the current score

	int GetCurrentScore() const { return mCurrScore.points; }  // Get the current score points
	int GetHighestScore() const ;                              // Get the highest score from the saved scores

	void SaveScores();  // Save scores to the file
	void LoadScores();  // Load scores from the file

	std::vector<Score> GetParsedScores() { return mParsedScores; }
	size_t GetParsedScoresSize() { return mParsedScores.size(); }

private:
	std::string EncryptDecrypt(const std::string& data);  // Encrypt or decrypt data
	std::string SerializeScores();                        // Serialize scores into a string
	void DeserializeScores(const std::string& data);      // Deserialize scores from a string

	void SortScoresVec();  // Sort the scores vector

	Score mCurrScore;                  // Current active score
	std::vector<Score> mParsedScores;  // Vector of all parsed scores
};