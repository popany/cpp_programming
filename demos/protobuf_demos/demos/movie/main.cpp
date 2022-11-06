#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/text_format.h"
#include "movie.pb.h"

demo::Movies gen_movies_msg() {
    demo::Movies movies{};
    {
        auto* new_movie = movies.add_movie();
        new_movie->set_title("The Shawshank Redemption");
        new_movie->set_rank(1);
        new_movie->set_director("Frank Darabont");
        *(new_movie->add_writers()) = "Stephen King";
        *(new_movie->add_top_cast()) = "Tim Robbins";
        *(new_movie->add_top_cast()) = "Morgan Freeman";
        *(new_movie->add_top_cast()) = "Bob Gunton";
    }
    {
        auto* new_movie = movies.add_movie();
        new_movie->set_title("The Godfather");
        new_movie->set_rank(2);
        new_movie->set_director("Francis Ford Coppola");
        *(new_movie->add_writers()) = "Mario Puzo";
        *(new_movie->add_writers()) = "Francis Ford Coppola";
        *(new_movie->add_top_cast()) = "Marlon Brando";
        *(new_movie->add_top_cast()) = "Al Pacino";
        *(new_movie->add_top_cast()) = "James Caan";
    }
    {
        auto* new_movie = movies.add_movie();
        new_movie->set_title("The Dark Knight");
        new_movie->set_rank(3);
        new_movie->set_director("Christopher Nolan");
        *(new_movie->add_writers()) = "Jonathan Nolan";
        *(new_movie->add_writers()) = "Christopher Nolan";
        *(new_movie->add_writers()) = "David S. Goyer";
        *(new_movie->add_top_cast()) = "Christian Bale";
        *(new_movie->add_top_cast()) = "Heath Ledger";
        *(new_movie->add_top_cast()) = "Aaron Eckhart";
    }
    return movies;
}

bool write_to_text_file(const std::string& file_path, const google::protobuf::Message& msg) {
    int fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd < 0) {
        std::cout << "unable to open file " << file_path << " to write";
        return false;
    }

    using google::protobuf::TextFormat;
    using google::protobuf::io::FileOutputStream;
    using google::protobuf::io::ZeroCopyOutputStream;
    ZeroCopyOutputStream *output = new FileOutputStream(fd);
    bool success = TextFormat::Print(msg, output);
    delete output;
    close(fd);
    return success;
}

bool read_from_text_file(const std::string& file_path, google::protobuf::Message* msg) {
    using google::protobuf::TextFormat;
    using google::protobuf::io::FileInputStream;
    using google::protobuf::io::ZeroCopyInputStream;
    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd < 0) {
        std::cout << "failed to open file " << file_path;
        return false;
    }

    ZeroCopyInputStream *input = new FileInputStream(fd);
    bool success = TextFormat::Parse(input, msg);
    if (!success) {
        std::cout << "failed to parse file " << file_path << " as text proto";
    }
    delete input;
    close(fd);
    return success;
}

bool write_to_bin_file(const std::string& file_path, const google::protobuf::Message& msg) {
    std::fstream output(file_path, std::ios::out | std::ios::trunc | std::ios::binary);
    return msg.SerializeToOstream(&output);
}

bool read_from_bin_file(const std::string& file_path, google::protobuf::Message* msg) {
    std::fstream input(file_path, std::ios::in | std::ios::binary);
    if (!input.good()) {
        std::cout << "failed to open file " << file_path;
        return false;
    }
    if (!msg->ParseFromIstream(&input)) {
        std::cout << "failed to parse file " << file_path;
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "no command" << std::endl;
        return 1;
    }
    const std::string command_write_text = "write_text";
    const std::string command_read_text = "read_text";
    const std::string command_write_bin = "write_bin";
    const std::string command_read_bin = "read_bin";
    if (argc < 3) {
        std::cout << "no file" << std::endl;
        return 1;
    }
    if (std::string(argv[1]) == command_write_text) {
        auto msg = gen_movies_msg();
        const std::string file_path = argv[2];
        if (write_to_text_file(file_path, msg)) {
            return 0;
        } else {
            return 1;
        }
    } else if (std::string(argv[1]) == command_read_text) {
        const std::string file_path = argv[2];
        demo::Movies msg{};
        if (read_from_text_file(file_path, &msg)) {
            msg.PrintDebugString();
            return 0;
        } else {
            return 1;
        }
    } else if (std::string(argv[1]) == command_write_bin) {
        auto msg = gen_movies_msg();
        const std::string file_path = argv[2];
        if (write_to_bin_file(file_path, msg)) {
            return 0;
        } else {
            return 1;
        }
    } else if (std::string(argv[1]) == command_read_bin) {
        const std::string file_path = argv[2];
        demo::Movies msg{};
        if (read_from_bin_file(file_path, &msg)) {
            msg.PrintDebugString();
            return 0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
    
    return 0;
}
