#include <symengine/polys/uratpoly.h>

namespace SymEngine
{

URatPoly::URatPoly(const RCP<const Basic> &var, URatDict &&dict)
    : USymEnginePoly(var, std::move(dict))
{
    SYMENGINE_ASSERT(is_canonical(poly_))
}

std::size_t URatPoly::__hash__() const
{
    std::size_t seed = URATPOLY;

    seed += var_->hash();
    for (const auto &it : poly_.dict_) {
        std::size_t temp = URATPOLY;
        hash_combine<unsigned int>(temp, it.first);
        hash_combine<long long int>(temp, mp_get_si(get_num(it.second)));
        hash_combine<long long int>(temp, mp_get_si(get_den(it.second)));
        seed += temp;
    }
    return seed;
}

bool divides_upoly(const URatPoly &a, const URatPoly &b,
                   const Ptr<RCP<const URatPoly>> &out)
{
    if (!(a.get_var()->__eq__(*b.get_var())))
        throw std::runtime_error("Error: variables must agree.");

    auto a_poly = a.get_poly();
    auto b_poly = b.get_poly();
    if (a_poly.size() == 0)
        return false;

    map_uint_mpq res;
    URatDict tmp;
    rational_class q, r;
    unsigned int a_deg, b_deg;

    while (b_poly.size() >= a_poly.size()) {
        a_deg = a_poly.degree();
        b_deg = b_poly.degree();
        q = b_poly.get_lc() / a_poly.get_lc();
        res[b_deg - a_deg] = q;
        URatDict tmp = URatDict({{b_deg - a_deg, q}});
        b_poly -= (a_poly * tmp);
    }

    if (b_poly.empty()) {
        *out = URatPoly::from_dict(a.get_var(), std::move(res));
        return true;
    } else {
        return false;
    }
}

} // SymEngine
