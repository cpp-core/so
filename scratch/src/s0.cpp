// Copyright (C) 2022, 2023 by Mark Melton
//

#include "core/util/tool.h"
#include "core/chrono/stopwatch.h"

struct ListNode {
    ListNode() : val(0), next(nullptr) { }
    ListNode(int x) : val(x), next(nullptr) { }
    ListNode(int x, ListNode *n) : val(x), next(n) { }
    int val;
    ListNode *next;
};

ListNode *swap_vals(ListNode *head) {
    auto curr = head;
    while (curr and curr->next) {
	std::swap(curr->val, curr->next->val);
	curr = curr->next->next;
    }
    return head;
}

ListNode *swap_ptrs(ListNode *head) {
    auto ptr = &head;
    while (ptr and *ptr and (*ptr)->next) {
	auto& p = *ptr;
	auto& q = p->next;
	auto& r = q->next;
	std::swap(p, q);
	std::swap(q, r);
	ptr = q ? &q : nullptr;
    }
    return head;
}

ListNode* swap_vals_sharma(ListNode* head) {
    ListNode* p = head;
    int temp = 0;
    while(p!= NULL && p->next!=NULL)
    {
	temp = p->val;
	p->val = p->next->val;
	p->next->val = temp;
	p = p->next->next;
    }
    return head;
}

ListNode* swap_ptrs_sharma(ListNode* head) {
    ListNode* p = head;
    ListNode* q = head;
    ListNode* r = head;
    int flag = 0;
    while(p!= NULL && p->next!=NULL)
    {
	if(flag == 0)
	{
	    flag = 1;
	    q = p->next;
	    p->next = q->next;
	    q->next = p;
	    head = q;
	    p = q->next->next;
	    r = q->next;
	    continue;
	}
	q = p->next;
	p->next = q->next;
	q->next = p;
	r->next = q;
	p = q->next->next;
	r = q->next;
    }
    return head;
}

auto create_list(int size) {
    ListNode *head = new ListNode(0);
    ListNode *curr = head;
    for (auto i = 1; i < 10; ++i) {
	auto node = new ListNode(i);
	curr->next = node;
	curr = node;
    }
    return head;
}

void print_list(ListNode *lst) {
    for (auto node = lst; node; node = node->next)
	cout << node->val << " ";
    cout << endl;
}

template<class Work>
void measure(std::string_view desc, size_t n, Work&& work) {
    auto lst = create_list(n);
    chron::StopWatch timer;
    size_t ns{};
    for (auto i = 0; i < 10; ++i) {
	timer.mark();
	lst = work(lst);
	ns += timer.elapsed_duration<std::chrono::nanoseconds>().count();
    }
    cout << fmt::format("{} {} ns", desc, ns / 10) << endl;
}

int tool_main(int argc, const char *argv[]) {
    {
	auto lst = create_list(10);
	lst = swap_vals(lst);
	print_list(lst);
    }

    {
	auto lst = create_list(10);
	lst = swap_ptrs(lst);
	print_list(lst);
    }

    {
	auto lst = create_list(10);
	lst = swap_vals_sharma(lst);
	print_list(lst);
    }

    {
	auto lst = create_list(10);
	lst = swap_ptrs_sharma(lst);
	print_list(lst);
    }

    measure("swap_vals-sharma     ", 1'000'000, [](auto head) { return swap_vals_sharma(head); });
    measure("swap_ptrs-sharma     ", 1'000'000, [](auto head) { return swap_ptrs_sharma(head); });
    measure("swap_vals-random-bits", 1'000'000, [](auto head) { return swap_vals(head); });
    measure("swap_ptrs-random-bits", 1'000'000, [](auto head) { return swap_ptrs(head); });
    return 0;
}
