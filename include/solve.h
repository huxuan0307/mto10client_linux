#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <set>
#include <type_traits>
#include <memory>
#include <algorithm>
#include <assert.h>
using std::sort;
using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::vector;
using std::queue;
using std::pair;
using std::move;
using std::set;
using std::unique_ptr;
enum class SolveType
{
	rand, manu, Algo1, Algo2, Algo3, Algo4, Algo10
};


using gamemap_t = vector<vector<int>>;
using gamemask_t = vector<vector<bool>>;

struct AnalysisResult
{
	pair<int, int> coord;
	int num_is_alone;
	int num_not_alone;
	int num_block_not_alone;
	int num_is_blank;
	float weight_of_alone;
	float rank;
	void showDetail ()
	{
		cout << "num_is_alone: " << num_is_alone << endl
			<< "num_not_alone: " << num_not_alone << endl
			<< "num_block_not_alone: " << num_block_not_alone << endl
			<< "num_is_blank: " << num_is_blank << endl
			;
	}
};


struct GameSubMap
{
	GameSubMap (int row, int col, int value)
		: data{ gamemask_t (row, vector<bool> (col, false)) }, value{ value }
	{ }
	GameSubMap (GameSubMap const& right)
	{
		data = right.data;
		mean = right.mean;
		num = right.num;
		value = right.value;
	}

	GameSubMap (GameSubMap&& right)noexcept
	{
		data = move (right.data);
		mean = right.mean;
		num = right.num;
		value = right.value;
	}

	void showSubMap ()
	{
		const auto& row = data.size ();
		const auto& col = data[0].size ();
		for (int i = 0; i < row; ++i) {
			for (int j = 0; j < col; ++j) {
				cout << (data[i][j] ? value : 0) << ' ';
			}
			endl (cout);
		}
		endl (cout);
	}
	gamemask_t data;
	pair<float, float> mean;
	int num;
	int value;
};

struct GameSubMaps
{
	GameSubMaps (int row, int col)
		:row{ row }, col{ col }
	{}
	vector<GameSubMap> data;
	void showSubMaps ()
	{
		for (auto& m : data) {
			for (int i = 0; i < row; ++i) {
				for (int j = 0; j < col; ++j) {
					cout << (m.data[i][j] ? m.value : 0) << ' ';
				}
				endl (cout);
			}
			endl (cout);
		}
	}
	int row;
	int col;
};

struct solve
{

	solve (vector<vector<int>>& gamemap_, int maxvalue_, SolveType sType_)
		:_gamemap( gamemap_ ), maxvalue{ maxvalue_ }, sType{ sType_ }
	{
		row = static_cast<int>(_gamemap.size ());
		col = static_cast<int>(_gamemap.empty () ? 0 : _gamemap[0].size ());

	}

	string operator()()
	{
		switch (sType) {
		case SolveType::rand:
			return RandomAlgo ();
			break;
		case SolveType::manu:
			return manuAlgo ();
			break;
		case SolveType::Algo1:
			return Algo1 ();
			break;
		case SolveType::Algo2:
			return Algo2 ();
			break;
		case SolveType::Algo3:
			return Algo3 ();
			break;
		case SolveType::Algo10:
			return Algo10 ();
			break;
		default:
			break;
		}
	}

	string Algo10 ()
	{
		auto p = Algo10_help ();
		return string{ static_cast<char>('A' + p.first), static_cast<char>('0' + p.second) };
	}
	pair<int, int> Algo10_help ()
	{
		GameSubMaps submaps (row, col);
		int minValue = maxvalue;
		int maxcnt = 1;
		std::unique_ptr<GameSubMap> targetmap;
		gamemap_t mask = gamemap_t (row, vector<int> (col, 0));
		for (int r = 0; r < row; ++r) {
			for (int c = 0; c < col; ++c) {
				if (mask[r][c] == 0) {
					// has not been search
					GameSubMap gsm (row, col, _gamemap[r][c]);
					int cnt = bfs (this->_gamemap, mask, gsm.data, { r,c });
					if (cnt > 1) {
						gsm.num = cnt;
						if (gsm.value < minValue) {
							maxcnt = 1;
							minValue = gsm.value;
							if (gsm.num > maxcnt) {
								maxcnt = gsm.num;
								targetmap.reset (new GameSubMap (move (gsm)));
							}
						}
						else if (gsm.value == minValue) {
							if (gsm.num > maxcnt) {
								maxcnt = gsm.num;
								targetmap.reset (new GameSubMap (move (gsm)));
							}
						}
					}
				}
			}
		}
		vector<AnalysisResult> analysisResults;

		for (int r = row - 1; r >= 0; --r) {
			for (int c = 0; c < col; ++c) {
				if (targetmap->data[r][c]) {
					auto res = afterMerge (targetmap->data, { r,c });
					res.coord = { r,c };
					analysisResults.push_back (res);
				}
			}
		}
		sort (analysisResults.begin (), analysisResults.end (), [](AnalysisResult const& left, AnalysisResult const& right) {
			return left.rank < right.rank;
			});
		return analysisResults.front ().coord;
	}

	string RandomAlgo ()
	{
		char randrow = rand () % row + 'A';
		char randcol = rand () % col + '0';
		return string{ randrow, randcol };
	}

	string manuAlgo ()
	{
		string coord;
		while (true) {
			cout << "input a coordinate: ";
			cin >> coord;
			if (coord.size () == 2 && coord[0] >= 'A' && coord[0] <= 'A' - 1 + row &&
				coord[1] >= '1' && coord[1] <= '0' - 1 + col) {
				break;
			}
		}
		return coord;
	}

	string Algo1 ()
	{
		auto p = Algo1_help ();
		return string{ static_cast<char>('A' + p.first), static_cast<char>('0' + p.second)};
	}

	pair<int, int> Algo1_help ()
	{
		GameSubMaps submaps (row, col);
		int minValue = maxvalue;
		int maxcnt = 1;
		std::unique_ptr<GameSubMap> targetmap;
		gamemap_t mask = gamemap_t (row, vector<int> (col, 0));
		for (int r = 0; r < row; ++r) {
			for (int c = 0; c < col; ++c) {
				if (mask[r][c] == 0) {
					// has not been search
					GameSubMap gsm (row, col, _gamemap[r][c]);
					int cnt = bfs (_gamemap, mask, gsm.data, { r,c });
					if (cnt > 1) {
						gsm.num = cnt;
						if (gsm.value < minValue) {
							maxcnt = 1;
							minValue = gsm.value;
							if (gsm.num > maxcnt) {
								maxcnt = gsm.num;
								targetmap.reset (new GameSubMap (move (gsm)));
							}
						}
						else if (gsm.value == minValue) {
							if (gsm.num > maxcnt) {
								maxcnt = gsm.num;
								targetmap.reset (new GameSubMap (move (gsm)));
							}
						}
						//submaps.data.push_back (move (gsm));
					}
				}
			}
		}
		//submaps.showSubMaps ();
		//targetmap->showSubMap ();
		//abort ();
		for (int i = row-1; i >=0; --i) {
			for (int j = 0; j < col; ++j) {
				if (targetmap->data[i][j]) {
					return std::make_pair (i, j);
				}
			}
		}
	}

	// 尝试分析消除之后的碎片化程度来找合适的消除位置
	string Algo2 ()
	{
		auto p = Algo2_help ();
		return string{ static_cast<char>('A' + p.first), static_cast<char>('0' + p.second) };
	}

	pair<int, int> Algo2_help ()
	{
		gamemap_t mask = gamemap_t (row, vector<int> (col, 0));
		vector<AnalysisResult> analysisResults;
		for (int r = 0; r < row; ++r) {
			for (int c = 0; c < col; ++c) {
				if (mask[r][c] == 0) {
					GameSubMap gsm (row, col, _gamemap[r][c]);
					int cnt = bfs (this->_gamemap, mask, gsm.data, { r,c });
					if (cnt > 1) {
						//gsm.showSubMap ();
						for (int rr = 0; rr < row; ++rr) {
							for (int cc = 0; cc < col; ++cc) {
								if (gsm.data[rr][cc]) {
									analysisResults.push_back (afterMerge (gsm.data, { rr,cc }));

								}
							}
						}
					}
				}
			}
		}
		sort (analysisResults.begin (), analysisResults.end (), [](AnalysisResult const& left, AnalysisResult const& right) {
			return (left.num_is_alone < right.num_is_alone)
				|| (left.num_is_alone == right.num_is_alone && left.coord.first > right.coord.first);
			});

		return analysisResults.front ().coord;
	}

	string Algo3 ()
	{
		auto p = Algo3_help ();
		return string{ static_cast<char>('A' + p.first), static_cast<char>('0' + p.second) };
	}

	pair<int, int> Algo3_help ()
	{
		gamemap_t mask = gamemap_t (row, vector<int> (col, 0));
		vector<AnalysisResult> analysisResults;
		for (int r = 0; r < row; ++r) {
			for (int c = 0; c < col; ++c) {
				if (mask[r][c] == 0) {
					GameSubMap gsm (row, col, _gamemap[r][c]);
					int cnt = bfs (this->_gamemap, mask, gsm.data, { r,c });
					if (cnt > 1) {
						//gsm.showSubMap ();
						for (int rr = 0; rr < row; ++rr) {
							for (int cc = 0; cc < col; ++cc) {
								if (gsm.data[rr][cc]) {
									analysisResults.push_back (afterMerge (gsm.data, { rr,cc }));

								}
							}
						}
					}
				}
			}
		}
		sort (analysisResults.begin (), analysisResults.end (), [](AnalysisResult const& left, AnalysisResult const& right) {
			return left.weight_of_alone < right.weight_of_alone;
			});

		return analysisResults.front ().coord;
	}


	AnalysisResult afterMerge (gamemask_t const& mask, pair<int, int>const& p)
	{
		gamemap_t gameMapTmp (_gamemap);
		int value = gameMapTmp[p.first][p.second];
		Merge (gameMapTmp, mask, p);
		FallDown (gameMapTmp);
		//showGameMap (gameMapTmp);
		auto res = Analysis (gameMapTmp);
		res.coord = p;
		//res.showDetail ();
		return res;
	}

	AnalysisResult Analysis (gamemap_t const& gamemap)
	{
		AnalysisResult res{};
		set<pair<int, int>> aloneSet;
		gamemap_t mask = gamemap_t (row, vector<int> (col, 0));
		for (int r = 0; r < row; ++r) {
			for (int c = 0; c < col; ++c) {
				GameSubMap gsm (row, col, gamemap[r][c]);
				int cnt = bfs (gamemap, mask, gsm.data, { r,c });
				if (cnt > 1) {
					if (gamemap[r][c] != 0) {
						res.num_block_not_alone++;
						res.num_not_alone += cnt;
					}
					else {
						res.num_is_blank += cnt;
					}
				}
				else if (cnt == 1) {
					res.num_is_alone++;
					res.weight_of_alone += pow (1.5, r) + abs (c - col / 2);
					aloneSet.insert ({ r,c });
				}
			}
		}
		res.rank = CalcuRank (gamemap, aloneSet);
		return res;
	}

	float CalcuRank (gamemap_t const& gamemap, set<pair<int, int>>const& aloneSet)
	{
		float Rank = 0;
		for (auto& p : aloneSet) {
			auto r = p.first, c = p.second;
			// horizontal
			if (c - 1 >= 0 && c < col && gamemap[r][c] && gamemap[r][c - 1]) {
				Rank += pow (abs (gamemap[r][c] - gamemap[r][c - 1]) + 1, 2) * (20 - gamemap[r][c]);
			}
			else if (c - 1 >= 0 && c < col && gamemap[r][c]) {
				Rank += pow (abs (gamemap[r][c]) + 1, 2) * (20 - gamemap[r][c]);
			}
			if (c >= 0 && c + 1 < col && gamemap[r][c] && gamemap[r][c + 1]) {
				Rank += pow (abs (gamemap[r][c] - gamemap[r][c + 1]) + 1, 2) * (20 - gamemap[r][c]);
			}
			else if (c >= 0 && c + 1 < col && gamemap[r][c]) {
				Rank += pow (abs (gamemap[r][c]) + 1, 2) * (20 - gamemap[r][c]);
			}

			if (r >= 0 && r + 1 < row && gamemap[r][c] && gamemap[r + 1][c]) {
				if (gamemap[r][c] < gamemap[r + 1][c]) {
					Rank += pow (abs (gamemap[r + 1][c] - gamemap[r][c]) + 1, 2) * (20 - gamemap[r][c]);
				}
				else {
					Rank += pow (abs (gamemap[r + 1][c] - gamemap[r][c]) + 1, 3) * (20 - gamemap[r][c]);
				}
			}
			if (r - 1 >= 0 && r < row && gamemap[r][c] && gamemap[r - 1][c]) {
				if (gamemap[r][c] < gamemap[r - 1][c]) {
					Rank += pow (abs (gamemap[r - 1][c] - gamemap[r][c]) + 1, 3) * (20 - gamemap[r][c]);
				}
				else {
					Rank += pow (abs (gamemap[r - 1][c] - gamemap[r][c]) + 1, 2) * (20 - gamemap[r][c]);
				}
			}
		}

		return Rank;
	}

	void Merge (gamemap_t& gamemap, gamemask_t const& mask, pair<int, int>const& p)
	{
		int value = gamemap[p.first][p.second];
		for (int r = 0; r < row; ++r) {
			for (int c = 0; c < col; ++c) {
				if (mask[r][c]) {
					gamemap[r][c] = 0;
				}
			}
		}
		// check if value at p is cleared
		assert (gamemap[p.first][p.second] == 0);
		gamemap[p.first][p.second] = value + 1;
	}

	void FallDown (gamemap_t& gamemap)
	{
		// the row that block above can fall onto
		int fallplanerow;
		for (int c = 0; c < col; ++c) {
			fallplanerow = row;
			for (int r = row - 1; r >= 0; --r) {
				// find first 0
				if (gamemap[r][c] != 0) {
					fallplanerow = r;
				}
				else {
					break;
				}
			}
			for (int r = fallplanerow - 1; r >= 0; --r) {
				if (gamemap[r][c] != 0) {
					gamemap[--fallplanerow][c] = gamemap[r][c];
					gamemap[r][c] = 0;
				}

			}
		}

	}

	int bfs (gamemap_t const& gamemap, gamemap_t& mask, gamemask_t& m, pair<int, int> point0)
	{
		queue<pair<int, int>> q;
		const auto& value = gamemap[point0.first][point0.second];
		if (mask[point0.first][point0.second] == 1)
			return 0;
		q.push (point0);
		set<pair<int, int>> s;
		while (!q.empty ()) {
			auto& r = q.front ().first;
			auto& c = q.front ().second;
			mask[r][c] = 1;
			m[r][c] = true;
			s.insert ({ r,c });
			// up
			if (r > 0 && !mask[r - 1][c] && gamemap[r - 1][c] == value) {
				q.emplace (r - 1, c);
			}
			// down
			if (r + 1 < row && !mask[r + 1][c] && gamemap[r + 1][c] == value) {
				q.emplace (r + 1, c);
			}
			// left
			if (c > 0 && !mask[r][c - 1] && gamemap[r][c - 1] == value) {
				q.emplace (r, c - 1);
			}
			// right
			if (c + 1 < col && !mask[r][c + 1] && gamemap[r][c + 1] == value) {
				q.emplace (r, c + 1);
			}
			q.pop ();
		}
		return (int)s.size ();
	}

	vector<vector<int>>& _gamemap;

	int row;
	int col;
	int maxvalue;
	SolveType sType;
};