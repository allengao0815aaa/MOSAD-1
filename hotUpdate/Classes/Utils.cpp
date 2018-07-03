#include "Utils.h"
#include "json\writer.h"
#include "cocos2d.h"
#include <string>

USING_NS_CC;

std::string serializeRapidjsonDocument(const rapidjson::Document &doc) {
  rapidjson::StringBuffer buffer;
  buffer.Clear();
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);

  return std::string(buffer.GetString());
}

std::string intToStr(int i) {
  char buf[32] = {};
  sprintf(buf, "%d", i);
  return std::string(buf);
}

bool writeFile(const char * path, const char * data, int length) {
  FILE *file = fopen(path, "wb");
  if (file) {
    fwrite(data, sizeof(char), length, file);
    printf("Saved %d bytes in %s\n", length, path);
    fclose(file);
  } else {
    printf("Save failed\n");
    return false;
  }
  return true;
}

std::string joinStrings(const std::vector<std::string>& strs, const std::string & delim) {
  std::string ret;
  for (int i = 0; i < strs.size(); ++i) {
    if (i == 0) ret += strs[i];
    else ret += delim + strs[i];
  }
  return ret;
}

void splitString(const std::string &str, const std::string &delim, std::vector<std::string>& res) {
  if (str.empty()) return;
  res.clear();
  int offset = 0;
  std::string::size_type find_res = 0;
  while ((find_res = str.find(delim, offset)) != std::string::npos) {
    res.push_back(str.substr(offset, find_res - offset));
    offset = find_res + delim.size();
  }
  res.push_back(str.substr(offset));
}

void save(const std::string &cardName, const std::string &path) {
	auto dbpath = FileUtils::getInstance()->getWritablePath() + "cards.sqlite";
	sqlite3 *pdb = NULL;
	std::string sql;
	int succeed = sqlite3_open(dbpath.c_str(), &pdb);
	if (succeed == SQLITE_OK) {
		sql = "INSERT OR IGNORE INTO cards(name, path) VALUES('" + cardName + "', '" + path + "')";
		sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	}
	sqlite3_close(pdb);
}

void load(std::vector<std::string> &names, std::vector<std::string> &deck) {
	auto path = FileUtils::getInstance()->getWritablePath() + "cards.sqlite";
	sqlite3 *pdb = NULL;
	std::string sql;
	int succeed = sqlite3_open(path.c_str(), &pdb);
	if (succeed == SQLITE_OK) {
		sql = "CREATE TABLE IF NOT EXISTS cards(ID INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE, path TEXT UNIQUE)";
		succeed = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
		sql = "SELECT * FROM cards";
		char **re;
		int r, c;
		sqlite3_get_table(pdb, sql.c_str(), &re, &r, &c, NULL);
		for (int i = 1; i <= r; ++i) {
			names.push_back(re[i * c + 1]);
			deck.push_back(re[i * c + 2]);
		}
		sqlite3_free_table(re);
	}
	sqlite3_close(pdb);
}

std::string loadVersion() {
	auto user = UserDefault::getInstance();
	auto str = user->getStringForKey("version");
	if (str == "") {
		user->setStringForKey("version", "1.0");
		return "1.0";
	} else {
		return str;
	}
}

void saveVersion(std::string version) {
	auto user = UserDefault::getInstance();
	user->setStringForKey("version", version);
}
