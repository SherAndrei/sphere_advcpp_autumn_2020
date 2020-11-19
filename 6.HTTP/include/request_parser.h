#ifndef HTTP_REQUEST_PARSER_H
#define HTTP_REQUEST_PARSER_H

#include <tuple>

namespace http {

struct Request;

class RequestParser {
 public:
    RequestParser();
    // Сбросить состояние
    void reset();
    // Результат парсинга
    enum result_type { good, bad, indeterminate };
    // Обрабатываем данные. Значение good = полный запрос был обработан,
    // bad = данные неверные, indeterminate = обрабатываем дальше.
    // Возвращаемый InputIterator показывает где остановился парсинг
    template <typename InputIterator>
    std::tuple<result_type, InputIterator> parse(Request& req,
                            InputIterator begin, InputIterator end) {
        while (begin != end) {
            result_type result = consume(req, *begin++);
            if (result == good || result == bad)
                return std::make_tuple(result, begin);
        }
        return std::make_tuple(indeterminate, begin);
    }

 private:
    // Обработать следующий символ
    result_type consume(Request& req, char input);

    static bool is_char(int c);
    static bool is_ctl(int c);
    static bool is_tspecial(int c);
    static bool is_digit(int c);

    // Текущее состояние.
    enum state {
        method_start,
        method,
        uri,
        http_version_h,
        http_version_t_1,
        http_version_t_2,
        http_version_p,
        http_version_slash,
        http_version_major_start,
        http_version_major,
        http_version_minor_start,
        http_version_minor,
        expecting_newline_1,
        header_line_start,
        header_lws,
        header_name,
        space_before_header_value,
        header_value,
        expecting_newline_2,
        expecting_newline_3
    } state_;
};

}  // namespace http

#endif  // HTTP_REQUEST_PARSER_H
