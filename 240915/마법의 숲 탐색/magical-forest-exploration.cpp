/*

마법의 숲 탐색

R행 C열의 격자 형태
K명의 정령이 각자 골렘을 타고 숲을 탐색

골렘 이동 우선 순위
1. 남쪽으로 한 칸 내려감
2. 서쪽 방향으로 회전하면서 내려감 
3. 동쪽 방향으로 회전하면서 내려감

골렘이 더이상 남쪽으로 이동할 수 없으면 정령이 상하좌우 인접한 칸으로 이동 가능
현재 타고 있는 골렘의 출구가 다른 골렘과 인접하고 있으면 다른 골렘으로 이동 가능
가장 남쪽으로 이동하면 종료

※ 골렘이 최대한 남쪽으로 이동했지만 몸 일부가 숲을 벗어나면
	-> 숲에 위치한 모든 골렘이 숲을 빠져나가고 다음 골렘부터 새롭게 탐색 시작
	-> 해당하는 정령은 기록x

<입력>
row col 정령의 수
출발 골렘출구


0: 북
1: 동
2: 남
3: 서
*/

#include <iostream>
#include <queue>
using namespace std;

int row, col; //마법의 숲 크기
int go_cnt; //정령의 수
int map[73][73]; //마법의 숲
int start, golem; //출발 열, 골렘의 출구 방향
int result;
int cnt;
bool visited[73][73];

int d[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} }; //북동남서

queue <pair<int, int>> q; // {row, col}

void print() {
	for (int r = 1; r <= row; r++) {
		for (int c = 1; c <= col; c++) {
			cout.width(5);
			cout << map[r][c] << " ";
		}
		cout << endl;
	}

}

//마법의 숲 크기와 정령의 수 입력 받음
void input() {
	cin >> row >> col >> go_cnt;
	row += 2;
	col += 2;
}

//초기화
void reset() {
	for (int r = 0; r <= row; r++)
		map[r][0] = 1;
	for (int c = 0; c <= col; c++)
		map[0][c] = 0;
	for (int r = 1; r <= row; r++)
		for (int c = 1; c <= col; c++) 
			map[r][c] = 0;


}

void reset_visited() {
	for (int r = 1; r <= row; r++)
		for (int c = 1; c <= col; c++)
			visited[r][c] = false;
}

//정령의 현재 위치에서 BFS 시작
int BFS(int r, int c) {
	reset_visited();
	q.push({ r, c });
	visited[r][c] = true;

	int max_row = r;

	while (!q.empty()) {

		int x = q.front().first;
		int y = q.front().second;

		q.pop();

		for (int idx = 0; idx < 4; idx++) {
			int nx = x + d[idx][0];
			int ny = y + d[idx][1];

			if (nx < 1 || nx > row || ny < 1 || ny > col) continue;
			if (visited[nx][ny]) continue;
			if (map[nx][ny] == 0) continue;



			//다른 골렘으로 이동
			if (map[x][y] > 1000) {
				max_row = max(max_row, nx);
				visited[nx][ny] = true;
				q.push({ nx, ny });
				continue;
			}

			//같은 골렘 내에서 자유롭게 이동 가능
			if ((map[x][y] % 1000) == (map[nx][ny] % 1000)) {
				max_row = max(max_row, nx);

				visited[nx][ny] = true;
				q.push({ nx, ny });
				continue;
			}
		}
	}

	return max_row;

}


bool can_move_south(int r, int c) {
	if (r + 1 >= row) return false;


	if (map[r + 2][c] > 0) return false;
	if (map[r + 1][c - 1] > 0) return false;
	if (map[r + 1][c + 1] > 0) return false;

	return true;
}

bool can_move_west(int r, int c) {
	if (r + 1 >= row || c - 2 <= 2) return false;

	//서쪽으로 한칸 이동 후 남쪽으로 한 칸 이동 가능한지 확인
	//서쪽으로 이동 가능한지 확인
	if (map[r][c - 2] > 0) return false;
	if (map[r - 1][c - 1] > 0) return false;
	if (map[r + 1][c - 1] > 0) return false;

	//남쪽으로 이동 가능한지 확인
	if (map[r + 1][c - 2] > 0) return false;
	if (map[r + 2][c - 1] > 0) return false;

	return true;
}

bool can_move_east(int r, int c) {
	if (r + 1 >= row || c + 2 > col) return false;

	//동쪽으로 한 칸 이동 후 남쪽으로 한 칸 이동 가능한지 확인
	//동쪽으로 이동 가능한지 확인
	if (map[r][c + 2] > 0) return false;
	if (map[r - 1][c + 1] > 0) return false;
	if (map[r + 1][c + 1] > 0) return false;

	//남쪽으로 이동 가능한지 확인
	if (map[r + 1][c + 2] > 0) return false;
	if (map[r + 2][c + 1] > 0) return false;

	return true;
}

void move() {

	//출발하는 열과 골렘의 출구 방향을 입력 받음
	cin >> start >> golem;
	cnt++;
	int n_row, n_col;
	n_col = start + 2;
	n_row = 0;

	//최대로 내려갈 때까지 반복
	while (1) {

		//남쪽으로 이동 가능한지 확인하기
		if (can_move_south(n_row, n_col)) {
			n_row++; //정령이 남쪽으로 한 칸 이동
			continue;
		}

		//서쪽으로 이동 가능한지 확인하기
		if (can_move_west(n_row, n_col)) {
			n_row++;
			n_col--;
			golem--;
			if (golem < 0) golem += 4;
			continue;
		}

		//동쪽으로 이동 가능한지 확인하기
		if (can_move_east(n_row, n_col)) {
			n_row++;
			n_col++;
			golem++;
			if (golem > 3) golem -= 4;
			continue;
		}

		//더이상 이동이 불가능함
		//골렘 일부가 숲 밖에 있음
		if (n_row <= 2) {
			reset();
			break;
		}

		map[n_row][n_col] = cnt;
		for (int idx = 0; idx < 4; idx++) {
			map[n_row + d[idx][0]][n_col + d[idx][1]] = cnt;
		}
		//출구 표시
		map[n_row + d[golem][0]][n_col + d[golem][1]] = 1000+cnt;
		//print();
		// cout << endl;

		//정령 이동 시작
		result += (BFS(n_row, n_col)-2);
		// cout << "result " << result << endl;

		break;
	}


}
int main() {

	input();

	for (int g = 1; g <= go_cnt; g++)
		move();

	cout << result << endl;


	return 0;
}