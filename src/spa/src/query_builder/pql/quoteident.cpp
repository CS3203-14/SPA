#include "query_builder/pql/quoteident.h"
#include <regex>
#include <string>
#include "query_builder/core/exceptions.h"
#include "utils/utils.h"
using namespace QE;

// Definitions for QuoteIdent
QuoteIdent::QuoteIdent(std::string quote_ident_) : quote_ident(quote_ident_) {
  Utils::trim(quote_ident);
  if (!std::regex_match(quote_ident, std::regex("[a-zA-Z](\\d|[a-zA-Z])*"))) {
    throw PQLParseException("Invalid QuoteIdent: " + quote_ident);
  }
}
