#ifndef EVL_BENCHMARK_HPP
#define EVL_BENCHMARK_HPP

#include "lib/eviltwin/observer.hpp"

#include "chrono_timer.hpp"
#include "benchmark.hpp"

#include <forward_list>
#include <algorithm>
#include <memory>
#include <vector>

namespace Benchmark
{

class Evl
{
    std::forward_list<obs::detail::UniversalPtr> reg;

    NOINLINE(void handler(Rng_t& rng))
    {
        volatile std::size_t a = rng(); (void)a;
    }

    using Subject = obs::Subject<void(Rng_t&)>;
    using Foo = Benchmark::Evl;

    static chrono_timer s_timer;

    public:

//------------------------------------------------------------------------------

    NOINLINE(static double construction(std::size_t N))
    {
        std::size_t count = 1;
        std::size_t elapsed = 0;
        const std::size_t limit = g_limit;

        using std::placeholders::_1;

        for (; elapsed < limit; ++count)
        {
            s_timer.reset();

            std::unique_ptr<Subject> subject(new Subject);
            std::vector<Foo> foo_array(N);

            elapsed += s_timer.count<Timer_u>();
        }
        return testsize_over_dt(N, limit, count);
    }

//------------------------------------------------------------------------------

    NOINLINE(static double destruction(std::size_t N))
    {
        Rng_t rng;
        std::size_t count = 1;
        std::size_t elapsed = 0;
        const std::size_t limit = g_limit;

        std::vector<std::size_t> randomized(N);
        std::generate(randomized.begin(), randomized.end(), IncrementFill());

        using std::placeholders::_1;

        for (; elapsed < limit; ++count)
        {
            std::shuffle(randomized.begin(), randomized.end(), rng);
            {
                std::unique_ptr<Subject> subject(new Subject);
                std::vector<Foo> foo_array(N);

                for (auto index : randomized)
                {
                    auto& foo = foo_array[index];
                    foo.reg.emplace_front(subject->registerObserver
                        (std::bind(&Foo::handler, &foo, _1)));
                }
                s_timer.reset();
            }
            elapsed += s_timer.count<Timer_u>();
        }
        return testsize_over_dt(N, limit, count);
    }

//------------------------------------------------------------------------------

    NOINLINE(static double connection(std::size_t N))
    {
        Rng_t rng;
        std::size_t count = 1;
        std::size_t elapsed = 0;
        const std::size_t limit = g_limit;

        std::vector<std::size_t> randomized(N);
        std::generate(randomized.begin(), randomized.end(), IncrementFill());
        
        using std::placeholders::_1;

        for (; elapsed < limit; ++count)
        {
            std::shuffle(randomized.begin(), randomized.end(), rng);

            Subject subject;
            std::vector<Foo> foo_array(N);

            s_timer.reset();
            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                foo.reg.emplace_front(subject.registerObserver
                    (std::bind(&Foo::handler, &foo, _1)));
            }
            elapsed += s_timer.count<Timer_u>();
        }
        return testsize_over_dt(N, limit, count);
    }

//------------------------------------------------------------------------------

    NOINLINE(static double emission(std::size_t N))
    {
        Rng_t rng;
        std::size_t count = 1;
        std::size_t elapsed = 0;
        const std::size_t limit = g_limit;

        std::vector<std::size_t> randomized(N);
        std::generate(randomized.begin(), randomized.end(), IncrementFill());
        
        using std::placeholders::_1;

        for (; elapsed < limit; ++count)
        {
            std::shuffle(randomized.begin(), randomized.end(), rng);

            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                foo.reg.emplace_front(subject.registerObserver
                    (std::bind(&Foo::handler, &foo, _1)));
            }
            s_timer.reset();
            subject(rng);
            elapsed += s_timer.count<Timer_u>();
        }
        return testsize_over_dt(N, limit, count);
    }

//------------------------------------------------------------------------------

    NOINLINE(static double combined(std::size_t N))
    {
        Rng_t rng;
        std::size_t count = 1;
        const std::size_t limit = g_limit;

        std::vector<std::size_t> randomized(N);
        std::generate(randomized.begin(), randomized.end(), IncrementFill());
        std::shuffle(randomized.begin(), randomized.end(), rng);
        
        using std::placeholders::_1;

        s_timer.reset();

        for (; s_timer.count<Timer_u>() < limit; ++count)
        {
            Subject subject;
            std::vector<Foo> foo_array(N);

            for (auto index : randomized)
            {
                auto& foo = foo_array[index];
                foo.reg.emplace_front(subject.registerObserver
                    (std::bind(&Foo::handler, &foo, _1)));
            }
            subject(rng);
        }
        return testsize_over_dt(N, limit, count);
    }
};

chrono_timer Evl::s_timer;

} // namespace Benchmark -------------------------------------------------------

#endif // EVL_BENCHMARK_HPP
