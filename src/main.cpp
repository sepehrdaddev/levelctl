#include <args.hxx>
#include <filesystem>
#include <iostream>
#include <leveldb/db.h>

#define VERSION "v0.1"
#define ERR(str) std::cerr << "ERROR: " << str << '\n'

static void version() { std::cout << "levelctl " << VERSION << '\n'; }

class database {
  leveldb::DB *__db{};
  leveldb::Options options{};
  leveldb::Status status{};

public:
  database(std::string path) {
    options.create_if_missing = true;
    options.write_buffer_size = 64 * 1024 * 1024;
    options.reuse_logs = true;
    options.compression = leveldb::kSnappyCompression;
    status = leveldb::DB::Open(options, path, &__db);
    status_check();
  }

  void put(const std::string &key, const std::string &value) {
    status = __db->Put(leveldb::WriteOptions(), key, value);
    status_check();
    std::cout << "OK\n";
  }

  void get(const std::string &key, std::string &value) {
    status = __db->Get(leveldb::ReadOptions(), key, &value);
    status_check();
  }

  void del(const std::string &key) {
    status = __db->Delete(leveldb::WriteOptions(), key);
    status_check();
    std::cout << "OK\n";
  }

  void dump() {
    leveldb::Iterator *it = __db->NewIterator(leveldb::ReadOptions());

    for (it->SeekToFirst(); it->Valid(); it->Next())
      std::cout << it->key().ToString() << ":" << it->value().ToString()
                << '\n';

    assert(it->status().ok());
    delete it;
  }

  ~database() { delete __db; }

private:
  void status_check() {
    if (!status.ok()) {
      ERR(status.ToString());
      exit(EXIT_FAILURE);
    }
  }
};

static void init_command(args::Subparser &parser) {
  args::Positional<std::string> path(parser, "path", "Path to database",
                                     args::Options::Required);
  parser.Parse();
  if (std::filesystem::exists(args::get(path))) {
    ERR("database already exists");
    exit(EXIT_FAILURE);
  }

  database db{args::get(path)};
}

static void put_command(args::Subparser &parser) {
  args::Positional<std::string> key(parser, "key", "Key to add",
                                    args::Options::Required);
  args::Positional<std::string> value(parser, "value", "Value of key",
                                      args::Options::Required);
  args::Positional<std::string> path(parser, "path", "Path to database",
                                     args::Options::Required);

  parser.Parse();

  if (!std::filesystem::exists(args::get(path))) {
    ERR("database not found");
    exit(EXIT_FAILURE);
  }

  database db{args::get(path)};
  db.put(args::get(key), args::get(value));
}

static void get_command(args::Subparser &parser) {
  args::Positional<std::string> key(parser, "key", "Key to get",
                                    args::Options::Required);
  args::Positional<std::string> path(parser, "path", "Path to database",
                                     args::Options::Required);

  parser.Parse();

  if (!std::filesystem::exists(args::get(path))) {
    ERR("database not found");
    exit(EXIT_FAILURE);
  }

  std::string value{};
  database db{args::get(path)};
  db.get(args::get(key), value);
  std::cout << args::get(key) << ':' << value << '\n';
}

static void del_command(args::Subparser &parser) {
  args::Positional<std::string> key(parser, "key", "Key to delete",
                                    args::Options::Required);
  args::Positional<std::string> path(parser, "path", "Path to database",
                                     args::Options::Required);

  parser.Parse();

  if (!std::filesystem::exists(args::get(path))) {
    ERR("database not found");
    exit(EXIT_FAILURE);
  }

  database db{args::get(path)};
  db.del(args::get(key));
}

static void dump_command(args::Subparser &parser) {
  args::Positional<std::string> path(parser, "path", "Path to database",
                                     args::Options::Required);

  parser.Parse();

  if (!std::filesystem::exists(args::get(path))) {
    ERR("database not found");
    exit(EXIT_FAILURE);
  }

  database db{args::get(path)};
  db.dump();
}

int main(int argc, const char *argv[]) {

  args::ArgumentParser parser("levelctl - Manage Leveldb Databases");

  args::Group commands(parser, "Available Commands");

  args::Command init(commands, "init", "Initialize database in the given path",
                     &init_command);

  args::Command put(commands, "put", "Add key to the given database",
                    &put_command);

  args::Command get(commands, "get", "Get value of key from database",
                    &get_command);

  args::Command del(commands, "del", "Delete the given key from database",
                    &del_command);

  args::Command dump(commands, "dump", "Dump the given database",
                     &dump_command);

  args::Group flags(parser, "Flags", args::Group::Validators::DontCare,
                    args::Options::Global);

  args::HelpFlag help(flags, "help", "Display this help menu", {'h', "help"});

  args::Flag ver(flags, "version", "Display levelctl version",
                 {'v', "version"});

  try {
    parser.ParseCLI(argc, argv);
  } catch (args::Help) {
    std::cout << parser;
  } catch (args::Error &e) {
    if (ver) {
      version();
      return EXIT_SUCCESS;
    }
    std::cerr << "ERROR: " << e.what() << '\n' << parser << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
