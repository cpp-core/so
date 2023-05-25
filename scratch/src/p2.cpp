// Copyright (C) 2022, 2023 by Mark Melton
//

#include <algorithm>
#include <iostream>

using std::cin, std::cout, std::endl;

void algo() {
    int n, k;
    cin >> n >> k;
    
    int a[n], s[n], sum = 0, ans = 0, ans1;
    for(int i = 0; i < n; i++)
	cin >> a[i], sum += a[i], s[i] = sum;
    
    if (k == n) {
        cout << sum << endl;
        return;
    }
    
    for (int i = n - 1; i >= 0; i--) {
        if(i - k >= 0)
            ans1 = s[i] - s[i-k];
        ans = std::max(ans, ans1);
    }
    
    cout << ans << endl;
}

int main(int argc, const char *argv[]) {
    int count, k;
    cin >> count >> k;

    int sum{};
    int arr[k];
    for (auto i = 0; i < k; ++i) {
	cin >> arr[i];
	sum += arr[i];
    }

    int max_sum{sum};
    for (auto i = k; i < count; ++i) {
	auto idx = i % k;
	sum -= arr[idx];
	cin >> arr[idx];
	sum += arr[idx];
	max_sum = std::max(max_sum, sum);
    }

    cout << max_sum << endl;
    return 0; 
}
