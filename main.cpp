#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>

using namespace std;

string StrToDomainStyle(string str) {
    reverse(str.begin(), str.end());
    str.push_back('.');
    return str;
}

class Domain {
public:
    Domain(string str)
        : data_(StrToDomainStyle(move(str)))
    { }

    bool operator==(const Domain& rhs) const {
        return data_ == rhs.data_;
    }

    bool operator<(const Domain& rhs) const {
        return data_ < rhs.data_;
    }

    bool IsSubdomain(const Domain& other) const {
        auto it = data_.begin();
        for (auto other_it = other.data_.begin(); other_it != other.data_.end(); ) {
            if (*it++ != *other_it++) {
                return false;
            }
        }
        return true;
    }

    const string& GetData() const {
        return data_;
    }
    
private:
    string data_;
};

class DomainChecker {
public:
    template <typename InputIt>
    DomainChecker(InputIt first, InputIt last)
        : domains_{first, last}
    {
        sort(domains_.begin(), domains_.end(),
            [](const Domain& lhs, const Domain& rhs) {
                return lhs.GetData() < rhs.GetData();
            });
        auto new_last = unique(domains_.begin(), domains_.end(),
                            [](const Domain& lhs, const Domain& rhs) {
                                return rhs.IsSubdomain(lhs);
                            });
        domains_.erase(new_last, domains_.end());
    }

    bool IsForbidden(const Domain& domain) {
        auto it = upper_bound(domains_.begin(), domains_.end(), domain);
        return it != domains_.begin() && domain.IsSubdomain(*(--it));
    }

    const vector<Domain>& GetData() const {
        return domains_;
    }

private:
    vector<Domain> domains_;
};

vector<Domain> ReadDomains(istream& input, size_t n) {
    vector<Domain> domains;
    for (size_t i = 0; i < n; ++i) {
        string str;
        getline(input, str);
        domains.push_back(Domain{str});
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

void TestDomain() {

using namespace std::literals;

    // constructor tests
    {
        Domain domain("ru"s);
        auto test_domain_data = domain.GetData();
        assert(test_domain_data == "ur."s);
    }

    {
        Domain domain("hawk.ru"s);
        auto test_domain_data = domain.GetData();
        assert(test_domain_data == "ur.kwah."s);
    }

    // operator== tests
    {
        Domain lhs_domain("tickets.hawk.ru"s);
        Domain rhs_domain("tickets.hawk.ru"s);
        assert(lhs_domain == rhs_domain);
        assert(lhs_domain == Domain("tickets.hawk.ru"s));
        assert(!(lhs_domain == Domain("vip.tickets.hawk.ru"s)));
        assert(!(lhs_domain == Domain("hawk.ru"s)));
    }

    // IsSubDomain tests
    {
        Domain domain("ru"s);
        Domain subdomain("hawk.ru"s);
        assert(subdomain.IsSubdomain(domain));
        assert(Domain{"tickets.hawk.ru"s}.IsSubdomain(subdomain));
        assert(Domain{"tickets.hawk.ru"s}.IsSubdomain(domain));
        assert(!Domain{"tickets.hawk.ru"s}.IsSubdomain(Domain{"black_hawk.ru"s}));
    }
}

void TestDomainChecker() {

using namespace std::literals;

    // constuctor tests
    {
        vector<Domain> domains = {Domain{"kz"s}, Domain{"com"s}, Domain{"hawk.ru"s}};
        DomainChecker domain_checker{domains.begin(), domains.end()};
        auto test_domain_checker_data = domain_checker.GetData();
        assert(test_domain_checker_data.size() == 3);
        assert(test_domain_checker_data.back() == Domain{"kz"s});
    }

    // IsForbidden tests
    {
        vector<Domain> domains = {Domain{"kz"s}, Domain{"com"s}, Domain{"hawk.ru"s}};
        DomainChecker domain_checker{domains.begin(), domains.end()};
        assert(domain_checker.IsForbidden(Domain{"kz"s}));
        assert(domain_checker.IsForbidden(Domain{"hawk.ru"s}));
        assert(domain_checker.IsForbidden(Domain{"tickets.hawk.ru"s}));
        assert(!domain_checker.IsForbidden(Domain{"black_hawk.ru"s}));
        assert(!domain_checker.IsForbidden(Domain{"ru"s}));
    }
}

int main() {
    TestDomain();
    TestDomainChecker();

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}