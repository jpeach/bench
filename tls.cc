#include <benchmark/benchmark_api.h>

#include <atomic>
#include <system_error>

#include <pthread.h>
#include <unistd.h>

static unsigned ncpu()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

struct PthreadKey : public ::benchmark::Fixture
{
    void SetUp(const ::benchmark::State& state)
    {
        if (count.fetch_add(1) == 0) {
            int err = pthread_key_create(&key, nullptr);
            if (err != 0) {
                throw std::system_error(err, std::generic_category(), "pthread_key_create failed");
            }
        }
    }

    void TearDown(const ::benchmark::State& state)
    {
        if (count.fetch_sub(1) == 1) {
            int err = pthread_key_delete(key);
            if (err != 0) {
                throw std::system_error(err, std::generic_category(), "pthread_key_delete failed");
            }
        }
    }

    pthread_key_t key;
    std::atomic<int> count;
};

BENCHMARK_F(PthreadKey, GetSpecific)(benchmark::State& state)
{
    while (state.KeepRunning()) {
	void * p = (void *)1;

	benchmark::DoNotOptimize(pthread_setspecific(key, p));
	benchmark::DoNotOptimize(p = pthread_getspecific(key));
    }
}

struct Keyword : public ::benchmark::Fixture
{
    void SetUp(const ::benchmark::State& state)
    {
    }

    void TearDown(const ::benchmark::State& state)
    {
    }

    static __thread void * key;
};

__thread void * Keyword::key;

BENCHMARK_F(Keyword, Local)(benchmark::State& state)
{
    while (state.KeepRunning()) {
	void * tmp;
	benchmark::DoNotOptimize(key = (void *)1);
	benchmark::DoNotOptimize(tmp = key);
    }
}

struct ThreadLocal : public ::benchmark::Fixture
{
    void SetUp(const ::benchmark::State& state)
    {
    }

    void TearDown(const ::benchmark::State& state)
    {
    }

    static thread_local void * key;
};

thread_local void * ThreadLocal::key;

BENCHMARK_F(ThreadLocal, Local)(benchmark::State& state)
{
    while (state.KeepRunning()) {
	void * tmp;
	benchmark::DoNotOptimize(key = (void *)1);
	benchmark::DoNotOptimize(tmp = key);
    }
}
BENCHMARK_REGISTER_F(PthreadKey, GetSpecific)->Threads(ncpu() * 4);
BENCHMARK_REGISTER_F(Keyword, Local)->Threads(ncpu() * 4);
BENCHMARK_REGISTER_F(ThreadLocal, Local)->Threads(ncpu() * 4);

BENCHMARK_MAIN();
