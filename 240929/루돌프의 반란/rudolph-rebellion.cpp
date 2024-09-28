/*

좌상단(1, 1)
M턴에 거쳐 게임 진행
1. 루돌프가 움직임
2. 1번부터 P번 산타가 움직임 (기절한 산타, 탈락한 산타 이동 불가)

루돌프는 가장 가까운 산타를 향해 1칸 돌진
가까운 산타가 2명 이상이면 r좌표, c좌표 순으로 큰 산타를 향해 돌진
상하좌우대각선 8방향으로 돌진 가능

루돌프에게 거리가 가까워지는 방향으로
다른 산타나 게임판 밖으로 움직이는 것 불가
움직일 수 있는 칸이 없으면 움직이지 않음
움직일 수 있더라도 루돌프와 가까워질 수 없으면 움직이지 않음
상하좌우 4방향으로 움직일 수 있음 (상우하좌 우선순위)

산타-루돌프 같은칸 -> 충돌발생
루돌프가 움직여서 충돌 발생 -> 산타는 C만큼의 점수를 얻음, C만큼 밀려남
산타가 움직여서 충돌 발생 -> 산타는 D만큼의 점수를 얻음 D만큼 밀려남

밀려난 위치가 게임 판 밖에면 탈락
다른 산타가 있으면 상호작용 발생

다른 산타 -> 1칸 밀려남(연쇄)
게임 판 밖으로 밀려나면 탈락

산타는 충돌 후 1턴 기절
기절 중 상호작용 가능
기절 중 루돌프가 돌진 가능

<종료>
턴이 끝남
산타 모두 탈락

탈락하지 않으면 매 턴 산타에게 1점 추가부여

<출력> 
각 산타가 얻은 최종 점수

*/

#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

int map_size; //게임판 사이즈
int map[51][51]; //게임판
bool visited[51][51];
int turn_cnt; //턴 수
int santa_cnt; //산타 수
int rudolph_str; //루돌프 힘
int santa_str; //산타 힘

int ru_row, ru_col; //루돌프 위치
int santa[31]; //산타 기록
pair<int, int> now_santa[31];
int fainting_santa[31]; //기절 여부
int now_turn;

int d[8][2] = { {1, 0}, {0, 1}, {0, -1}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
int d_santa[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} }; //상우하좌

bool game_flag; //모든 산타 탈락여부

void print_map() {

    for (int row = 1; row <= map_size; row++) {
        for (int col = 1; col <= map_size; col++)
            cout << map[row][col] << " ";
        cout << endl;
    }

    cout << "점수" << endl;
    for (int idx = 1; idx <= santa_cnt; idx++)
        cout << santa[idx] << " ";
    cout << endl;

}

//각 턴이 끝나면 살아있는 산타에게 1점 추가
void give_santa() {
     int cnt = 0;
    for (int idx = 1; idx <= santa_cnt; idx++){
        if (now_santa[idx].first != -1) santa[idx]++;
        else  cnt++;
    }

    if (cnt == santa_cnt) game_flag = true;
}

void reset_visited() {
    for (int row = 1; row <= map_size; row++)
        for (int col = 1; col <= map_size; col++)
            visited[row][col] = false;
}

// 루돌프가 가까운 산타를 탐색함
// 이동 방향 리턴
int find_santa() {

    queue<pair<int, int>> q;
    reset_visited();

    q.push({ ru_row, ru_col });
    visited[ru_row][ru_col] = true;

    while (!q.empty()) {
        int x = q.front().first;
        int y = q.front().second;

        q.pop();

        for (int idx = 0; idx < 8; idx++) {
            int nx = x + d[idx][0];
            int ny = y + d[idx][1];

            if (nx < 1 || nx > map_size || ny < 1 || ny > map_size) continue;
            if (visited[nx][ny]) continue;

            if (map[nx][ny] == 0) {
                q.push({ nx, ny });
                visited[nx][ny] = true;
                continue;
            }

            //가장 가까운 산타 발견
            if (map[nx][ny] > 0) {
                return idx;
            }
        }
    }

}

void move_rudolph() {
    int ru_di = find_santa();

    ru_row += d[ru_di][0];
    ru_col += d[ru_di][1];



    //루돌프가 이동한 곳에 산타가 있는 경우
    if (map[ru_row][ru_col] > 0) {
        
        // 산타에게 점수 추가
        int santa_num = map[ru_row][ru_col];
        santa[santa_num] += rudolph_str;

        //산타 기절
        fainting_santa[santa_num] = now_turn;

        //첫 충돌
        int r = ru_row;
        int c = ru_col;
        map[r][c] = 0;

        //루돌프 힘만큼 밀려남
        int nr = r + (d[ru_di][0] * rudolph_str);
        int nc = c + (d[ru_di][1] * rudolph_str);

        //산타 탈락
        if (nr < 1 || nr > map_size || nc < 1 || nc > map_size) {
            now_santa[santa_num] = { -1, -1 };
            return;
        }

        //연쇄작용 발생할 산타 기억
        int n_santa = map[nr][nc];

        map[nr][nc] = santa_num;
        now_santa[santa_num] = { nr, nc }; //산타 위치 갱신

        //연쇄작용 산타 갱신
        santa_num = n_santa;


        //더이상 밀려나는 산타가 없을 때까지 진행
        while (santa_num > 0) {

            nr += d[ru_di][0];
            nc += d[ru_di][1];

            //산타 탈락
            if (nr < 1 || nr > map_size || nc < 1 || nc > map_size) {
                //밀려나는 산타이므로 원래 자리에 이미 다른 산타가 들어가있음
                now_santa[santa_num] = { -1, -1 };
                break;
            }

            int n_santa = map[nr][nc];

            map[nr][nc] = santa_num;
            now_santa[santa_num] = { nr, nc }; //산타 위치 갱신

            //밀려나는 산타
            santa_num = n_santa;
        }
    }
}

void move_santa(int s_num) {

    //탈락한 산타 체크
    if (now_santa[s_num].first == -1) return;

    //기절한 산타는 턴을 넘어감
    if ((now_turn != 1 && fainting_santa[s_num] + 1 == now_turn) || fainting_santa[s_num] == now_turn) {
        return;
    }

    //루돌프가 상대적으로 어느 위치에 있는지 판단
    int ru_san_row = ru_row - now_santa[s_num].first;
    int ru_san_col = ru_col - now_santa[s_num].second;
    
    int now_dist = (ru_san_row * ru_san_row) + (ru_san_col) * (ru_san_col);

    //무조건 루돌프와 가까워지는 방향으로 이동
    //다른 산타가 있는 자리에는 이동 불가
    // 이동하는 위치에 산타 있으면 충돌
    //우선순위 상우하좌
    
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    for (int idx = 0; idx < 4; idx++) {
        int n_dist = pow(ru_san_row - d_santa[idx][0], 2) + pow(ru_san_col - d_santa[idx][1], 2);


        if (now_dist <= n_dist) continue;

        pq.push({ n_dist, idx });
    }
    
    while (!pq.empty()) {

        int idx = pq.top().second;

        //루돌프와 충돌 (거리가 0이됨)
        if (pq.top().first == 0) {
            //점수 획득
            santa[s_num] += santa_str;
            fainting_santa[s_num] = now_turn;

            //밀려남
            //첫 충돌
            int r = ru_row;
            int c = ru_col;
            map[now_santa[s_num].first][now_santa[s_num].second] = 0;

            //산타 힘만큼 밀려남
            int nr = r - (d_santa[idx][0] * santa_str);
            int nc = c - (d_santa[idx][1] * santa_str);

            //산타 탈락
            if (nr < 1 || nr > map_size || nc < 1 || nc > map_size) {
                now_santa[s_num] = { -1, -1 };
                return;
            }

            //연쇄작용 발생할 산타 기억
            int n_santa = map[nr][nc];

            map[nr][nc] = s_num;
            now_santa[s_num] = { nr, nc }; //산타 위치 갱신

            //연쇄작용 산타 갱신
            s_num = n_santa;

            //더이상 밀려나는 산타가 없을 때까지 진행
            while (s_num > 0) {

                nr -= d_santa[idx][0];
                nc -= d_santa[idx][1];
                //산타 탈락
                if (nr < 1 || nr > map_size || nc < 1 || nc > map_size) {
                    now_santa[s_num] = { -1, -1 };
                    break;
                }

                int n_santa = map[nr][nc];

                map[nr][nc] = s_num;
                now_santa[s_num] = { nr, nc }; //산타 위치 갱신

                //밀려나는 산타
                s_num = n_santa;

            }


            break;
        }

        int nx = now_santa[s_num].first + d_santa[idx][0];
        int ny = now_santa[s_num].second + d_santa[idx][1];

        pq.pop();

        if (nx < 1 || nx > map_size || ny < 1 || ny > map_size) continue;
        if (map[nx][ny] > 0)continue;

        //이동 가능한 경우
        map[now_santa[s_num].first][now_santa[s_num].second] = 0;
        map[nx][ny] = s_num;
        now_santa[s_num] = { nx, ny };
        break;
    }

}


int main() {

    cin >> map_size >> turn_cnt >> santa_cnt >> rudolph_str >> santa_str;
    
    //루돌프 초기 위치
    cin >> ru_row >> ru_col;

    //산타 초기 위치
    int santa_num, row, col;
    for (int idx = 1; idx <= santa_cnt; idx++) {
        cin >> santa_num >> row >> col;
        map[row][col] = santa_num;
        now_santa[santa_num] = { row, col };
    }
    
    
    //게임 시작
    while(now_turn < turn_cnt) {
        
        if (game_flag) break; //모든 산타 탈락, 게임 종료

        now_turn++;

        move_rudolph(); //루돌프 이동

        for (int s = 1; s <= santa_cnt; s++)
            move_santa(s); //각 산타 이동

        give_santa(); //점수 부여

    }


    for (int idx = 1; idx <= santa_cnt; idx++)
        cout << santa[idx] << " ";

    return 0;
}