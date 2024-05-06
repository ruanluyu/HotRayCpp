
#include "l0_def/s0_numerical.hpp"
#include "l0_def/s1_global.hpp"
#include "l0_def/s2_vec.hpp"
#include "l0_def/s3_bigint.hpp"
#include "l1_ray/s2_configs.hpp"
#include "l2_port/s0_portbase.hpp"
#include "l3_node/s0_nodebase.hpp"

#include <iostream>

bool test_enable_log = false;

#define BEGIN_LOG_STATUS_AREA(ENABLE,OLD_VARNAME) { auto OLD_VARNAME = test_enable_log; test_enable_log = (ENABLE);{
#define END_LOG_STATUS_AREA(OLD_VARNAME) }test_enable_log = OLD_VARNAME;}


#define TEST_OUTPUT(varname) BEGIN_LOG_STATUS_AREA(false, _hrdbg_testoutput) \
{auto _hrdbg_res = varname; cout << #varname << ": "  << _hrdbg_res << endl;} \
END_LOG_STATUS_AREA(_hrdbg_testoutput)

using error_signal_type = ui32;

using TestFunction = bool (*)();


#define DEFINE_TEST(NAME) extern error_signal_type NAME()

#define BEGIN_TEST(NAME) error_signal_type NAME(){ui32 check_point_pos = 0;

#define TEST_ASSERT(BOOL_EXPRESSION) BEGIN_LOG_STATUS_AREA(false, _hrdbg_assert) \
{check_point_pos++; if((BOOL_EXPRESSION)) {cout << "#" << check_point_pos <<" passed! Assertion contents: " <<  #BOOL_EXPRESSION << endl;} else {cout << "Assert failed: " <<  #BOOL_EXPRESSION << endl; return check_point_pos;}}\
END_LOG_STATUS_AREA(_hrdbg_assert)

#define END_TEST return 0;}

#define RUN_TEST(NAME) {auto res = NAME(); cout << #NAME << ": " << (res == 0 ? "passed": "failed") << ", signal: " << res << endl;}




using namespace std;
using namespace hr::def;
using namespace hr::ray;
using namespace hr::port;
using namespace hr::node;


std::unordered_map<void*, ui64> mem_size_list;

void print_mem_size_list() {
	ui64 total = 0;
	for (auto& kv : mem_size_list)
	{
		cout << kv.first << ": " << kv.second << endl;
		total += kv.second;
	}
	cout << "total not-released memory: " << total << endl;
}

void* test_alloc_func(const ui64& size) {
	auto newly = malloc(size);
	if (test_enable_log)
	{
		mem_size_list[newly] = size;
		BEGIN_LOG_STATUS_AREA(false, test_alloc);
		cout << "aloc: " << newly << " " << size << " bytes" << endl;
		END_LOG_STATUS_AREA(test_alloc);
	}
	return newly;
}

void test_free_func(void* const& ptr) {
	if (test_enable_log)
	{
		BEGIN_LOG_STATUS_AREA(false, test_free);
		cout << "free: " << ptr << endl;
		END_LOG_STATUS_AREA(test_free);
	}
	mem_size_list.erase(ptr);
	void* copy = ptr;
	free(copy);
}

void logger(const char* info)
{
	cout << info << endl;
}


DEFINE_TEST(numerical_test);
DEFINE_TEST(converter_test);
DEFINE_TEST(memory_alloc_test);
DEFINE_TEST(bigint_test);
DEFINE_TEST(objectbase_test);
DEFINE_TEST(port_test);
DEFINE_TEST(routine_test);

int main()
{
	test_enable_log = false;
	hr::def::mem::init_strategy(mem::ManagementConfigs{
		.memory_allocate = test_alloc_func,
		.memory_free = test_free_func
		});
	hr::def::init_log({ logger ,logger ,logger });
	hr::ray::init_ray();

	test_enable_log = true;

	//RUN_TEST(numerical_test);
	//RUN_TEST(converter_test);
	//RUN_TEST(memory_alloc_test);
	//RUN_TEST(bigint_test);
	//RUN_TEST(objectbase_test);
	RUN_TEST(port_test);
	RUN_TEST(routine_test);

	cout << "Benchmark all done. " << endl;
	test_enable_log = false;

	print_mem_size_list();
	int k = getchar();

	return 0;
}


BEGIN_TEST(routine_test)
{
	{
		
	}
}
END_TEST
BEGIN_TEST(port_test)
{
	{
		cout << "IP/OP test" << endl;
		auto ipt = CreateObject<InPort>();
		auto opt = CreateObject<OutPort>();

		opt->SetRayConfig(global_ray_configs.GetConfig("i8"));
		ipt->SetRayConfig(global_ray_configs.GetConfig("ui16"));

		TEST_ASSERT(opt->TryConnectTo(ipt));

		opt->GetDataRawPointer()->Set<i8>(-3i8);
		opt->Send();
		TEST_OUTPUT(ipt->GetDataRawPointer()->Get<ui16>());
		TEST_ASSERT(ipt->GetDataRawPointer()->Get<ui16>() == 0ui16);

		DestroyObject(ipt);
		DestroyObject(opt);
	}
	

	{
		cout << "IPA test" << endl;
		auto ipa = CreateObject<InPortArray>();
		ipa->SetSize(2);
		DestroyObject(ipa);
	}

	{
		cout << "IPC test" << endl;
		auto ipc = CreateObject<InPortCollection>();

		ipc->Add(CreateObject<InPort>());
		ipc->Add(CreateObject<InPortArray>());
		ipc->Add(CreateObject<InPortCollection>());

		DestroyObject(ipc);
	}
	
}
END_TEST


BEGIN_TEST(objectbase_test)
{
	{
		auto obj = CreateObject<ObjectBase>();
		auto child = CreateObject<ObjectBase>();

		obj->AddChild(child);

		obj->SetName("Parent object");
		child->SetName("Child object");

		TEST_OUTPUT(child->GetName());

		ObjectBase::Destroy(obj);

	}

}
END_TEST

BEGIN_TEST(bigint_test)
{
	bigint i1, i2;
	i1.set(65846i64);
	i2.set(18919i64);

	bigint i3 = i1 + i2;

	TEST_OUTPUT(i3 > i2);
	TEST_OUTPUT(i3 < i2);

}
END_TEST

BEGIN_TEST(memory_alloc_test)
{
	class TestClass {
	public:
		TestClass(int alloc_val):data(alloc_val) { TEST_OUTPUT(alloc_val); }
		~TestClass() { cout << "Destruct " << data << endl; }
		int get_data() { return data; }
	private:
		int data;
	};

	auto ptr = mem::hr_new<TestClass>(1);

	mem::hr_delete(ptr);

	hr_weak_ptr<TestClass> ptr7;
	{
		cout << "Entered TestClass scope. " << endl;
		auto ptr2 = hr_make_shared<TestClass>(16);
		auto ptr3 = hr_make_unique<TestClass>(17);
		auto ptr4 = std::move(ptr3);
		hr_shared_ptr ptr5 = std::move(ptr4);
		hr_shared_ptr ptr6 = ptr5;
		TEST_OUTPUT(ptr5->get_data());
		TEST_OUTPUT(ptr6->get_data());
		ptr7 = ptr6;
		TEST_OUTPUT(ptr7.lock()->get_data());
		cout << "Exiting TestClass scope. " << endl;
	}
	cout << "Exited TestClass scope. " << endl;
	auto ptr8 = ptr7.lock();
	TEST_ASSERT(ptr8 == nullptr);

}
END_TEST

BEGIN_TEST(converter_test)
{
	hr::ray::RayData datai8, datai16, datai32, datai64,
		dataui8, dataui16, dataui32, dataui64;


	struct DataInfo {
		RayData& data;
		const char* name;
	};
	DataInfo di_i8{ datai8, "i8" },
		di_i16{ datai16, "i16" }, 
		di_i32{ datai32, "i32" }, 
		di_i64{ datai64, "i64" }, 
		di_ui8{ dataui8, "ui8" }, 
		di_ui16{ dataui16, "ui16" }, 
		di_ui32{ dataui32, "ui32" }, 
		di_ui64{ dataui64, "ui64" };

	

	enum ActionType {
		Set,
		Convert
	};

	struct OneAction {
		ActionType actionType;
		union ActionUnion {
			struct SetInfo {
				DataInfo& data;
				RayData set_to_data;
			} set;
			struct ConvertInfo {
				DataInfo& from;
				DataInfo& to;
				RayData right_answer;
			} convert;
		} actionInfo;
	};


	OneAction actions[] = {
		{Set, {.set = {di_i8, RayData(16i8)}}},
		{Convert, {.convert = {di_i8, di_i16, RayData(16i16)}}},
		{Convert, {.convert = {di_i8, di_i32, RayData(16i32)}}},
		{Convert, {.convert = {di_i8, di_i64, RayData(16i64)}}},
		{Convert, {.convert = {di_i8, di_ui8, RayData(16ui8)}}},
		{Convert, {.convert = {di_i8, di_ui16, RayData(16ui16)}}},
		{Convert, {.convert = {di_i8, di_ui32, RayData(16ui32)}}},
		{Convert, {.convert = {di_i8, di_ui64, RayData(16ui64)}}},
		{Set, {.set = {di_i8, RayData(-16i8)}}},
		{Convert, {.convert = {di_i8, di_ui8, RayData(0)}}},
		{Convert, {.convert = {di_i8, di_ui16, RayData(0)}}},
		{Convert, {.convert = {di_i8, di_ui32, RayData(0)}}},
		{Convert, {.convert = {di_i8, di_ui64, RayData(0)}}},
	};

	for (auto& action : actions)
	{
		switch (action.actionType)
		{
		case Set:
			cout << "set " << action.actionInfo.set.data.name << endl;
			action.actionInfo.set.data.data = action.actionInfo.set.set_to_data;
			break;
		case Convert:
			{
				auto info = action.actionInfo.convert;
				cout << "convert from " << info.from.name << " to " << info.to.name << endl;
				auto from_config = global_ray_configs.GetConfig(info.from.name);
				auto to_config = global_ray_configs.GetConfig(info.to.name);
				global_ray_configs.GetConverter(from_config, to_config)->Apply(info.from.data, info.to.data);
			}
			break;
		}
	}
}
END_TEST


BEGIN_TEST(numerical_test)
{


	f32v2 v{ 1,2 };
	f32v2 vcopy;
	stringstream ss;

	ss << v;
	ss >> vcopy;



	TEST_OUTPUT(v);
	TEST_OUTPUT(vcopy);


	auto v1 = v + 2;
	auto v2 = 2 + v;
	auto v3 = v + v1;

	TEST_OUTPUT(v);
	TEST_OUTPUT(v1);
	TEST_OUTPUT(v2);
	TEST_OUTPUT(v3);

	auto v4 = v - 2;
	auto v5 = 2 - v;
	auto v6 = v - v1;

	TEST_OUTPUT(v4);
	TEST_OUTPUT(v5);
	TEST_OUTPUT(v6);

	auto v7 = v * 2;
	auto v8 = 2 * v;
	auto v9 = v * v1;

	TEST_OUTPUT(v7);
	TEST_OUTPUT(v8);
	TEST_OUTPUT(v9);

	auto v10 = v / 2;
	auto v11 = 2 / v;
	auto v12 = v / v1;

	TEST_OUTPUT(v10);
	TEST_OUTPUT(v11);
	TEST_OUTPUT(v12);

	TEST_OUTPUT(v++);
	TEST_OUTPUT(v);
	TEST_OUTPUT(++v);
	TEST_OUTPUT(v);

	TEST_OUTPUT(v--);
	TEST_OUTPUT(v);
	TEST_OUTPUT(--v);
	TEST_OUTPUT(v);

	for (auto& val : v)
	{
		TEST_OUTPUT(val);
	}

	f32cv2 cv{ f32c{1.0f, 2.0f}, f32c{-0.5f, -1.0f} };
	f32cv2 cv2;

	ss.clear();
	ss.str("");
	ss << cv;
	ss >> cv2;

	TEST_OUTPUT(cv);
	TEST_OUTPUT(cv2);
}
END_TEST


