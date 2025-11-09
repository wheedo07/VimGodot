# VimGodot 함수 레퍼런스

VimGodot에서 사용 가능한 모든 함수들을 카테고리별로 정리했습니다.

## 🚀 모션 함수 (MOTION)

### 기본 이동
- **`move_by_characters`**: 문자 단위 이동
  - `forward`: 전진/후진 (bool)
  - `one_line`: 한 줄 내에서만 이동 (bool)
  - `repeat`: 반복 횟수 (int)

- **`move_by_lines`**: 줄 단위 이동
  - `forward`: 위/아래 방향 (bool)
  - `line_wise`: 라인 단위 모드 (bool)
  - `to_first_char`: 첫 번째 문자로 이동 (bool)
  - `repeat`: 반복 횟수 (int)

### 페이지/화면 이동
- **`move_by_scroll`**: 스크롤 단위 이동 (Ctrl+U/D)
  - `forward`: 위/아래 방향 (bool)

- **`move_by_page`**: 페이지 단위 이동 (Ctrl+F/B)
  - `forward`: 위/아래 방향 (bool)

- **`move_to_top_line`**: 화면 최상단 이동 (H)
- **`move_to_bottom_line`**: 화면 최하단 이동 (L)
- **`move_to_middle_line`**: 화면 중앙 이동 (M)

### 줄 내 이동
- **`move_to_start_of_line`**: 줄 시작으로 이동 (0)
- **`move_to_end_of_line`**: 줄 끝으로 이동 ($)
  - `inclusive`: 포함적 선택 (bool)
  - `repeat`: 반복 횟수 (int)

- **`move_to_first_non_white_space_character`**: 첫 번째 비공백 문자로 이동 (^)
  - `change_line`: 다른 줄로 이동 (bool)
  - `repeat`: 반복 횟수 (int)

- **`move_to_column`**: 특정 컬럼으로 이동 (|)
  - `repeat`: 컬럼 번호 (int)

### 단어 이동
- **`move_by_words`**: 단어 단위 이동 (w/b/e)
  - `forward`: 전진/후진 (bool)
  - `word_end`: 단어 끝으로 이동 (bool)
  - `big_word`: 큰 단어 모드 (bool)
  - `inclusive`: 포함적 선택 (bool)
  - `repeat`: 반복 횟수 (int)

### 문서 이동
- **`move_to_line_or_edge_of_document`**: 특정 줄 또는 문서 끝으로 이동 (gg/G)
  - `forward`: 문서 끝/시작 (bool)
  - `to_jump_list`: 점프 리스트에 추가 (bool)
  - `repeat`: 줄 번호 (int)
  - `repeat_is_explicit`: 명시적 반복 (bool)

### 문자 검색
- **`move_to_next_char`**: 특정 문자로 이동 (f/F/t/T)
  - `forward`: 전진/후진 (bool)
  - `inclusive`: 포함적 선택 (bool)
  - `stop_before`: 문자 앞에서 정지 (bool)
  - `selected_character`: 검색할 문자 (string)
  - `repeat`: 반복 횟수 (int)

- **`repeat_last_char_search`**: 마지막 문자 검색 반복 (;/,)
  - 이전 `move_to_next_char` 설정 재사용

### 기호 매칭
- **`move_to_matched_symbol`**: 매칭되는 기호로 이동 (%)
  - `inclusive`: 포함적 선택 (bool)
  - `to_jump_list`: 점프 리스트에 추가 (bool)

### 검색 이동
- **`find_word_under_caret`**: 커서 아래 단어 검색 (*/＃)
  - `forward`: 전진/후진 (bool)

- **`find_again`**: 검색 반복 (n/N)
  - `forward`: 전진/후진 (bool)

### 텍스트 객체
- **`text_object`**: 텍스트 객체 선택 (iw/aw/i(/a( 등)
  - `inner`: 내부/전체 (bool)
  - `object`: 객체 타입 (string: w/(/)/{/"/')

### 기타 이동
- **`expand_to_line`**: 라인 확장 (V 모드용)
  - `repeat`: 반복 횟수 (int)

- **`go_to_bookmark`**: 북마크로 이동 (')
  - `selected_character`: 북마크 이름 (string)

## ⚔️ 오퍼레이터 함수 (OPERATOR)

### 기본 편집
- **`deletes`**: 삭제 (d)
  - `line_wise`: 라인 단위 삭제 (bool)

- **`yank`**: 복사 (y)
  - `line_wise`: 라인 단위 복사 (bool)

- **`change`**: 변경 (c)
  - `line_wise`: 라인 단위 변경 (bool)

- **`delete_and_enter_insert_mode`**: 삭제 후 Insert 모드 진입 (s)
  - `line_wise`: 라인 단위 삭제 (bool)

### 대소문자 변환
- **`change_case`**: 대소문자 변환 (u/U)
  - `lower`: 소문자로 변환 (bool)

- **`toggle_case`**: 대소문자 토글 (~)

## 🎯 액션 함수 (ACTION)

### 모드 전환
- **`enter_insert_mode`**: Insert 모드 진입 (i/a/o/I/A/O)
  - `insert_at`: 진입 위치 (string)
    - `"inplace"`: 현재 위치
    - `"after"`: 다음 위치
    - `"bol"`: 줄 시작
    - `"eol"`: 줄 끝
    - `"new_line_below"`: 아래 새 줄
    - `"new_line_above"`: 위 새 줄

- **`enter_visual_mode`**: Visual 모드 진입 (v/V)
  - `line_wise`: 라인 단위 선택 (bool)

### 편집 작업
- **`paste`**: 붙여넣기 (p/P)
  - `after`: 뒤에 붙여넣기 (bool)
  - `repeat`: 반복 횟수 (int)

- **`undo`**: 실행취소 (u)
  - `repeat`: 반복 횟수 (int)

- **`redo`**: 재실행 (Ctrl+R)
  - `repeat`: 반복 횟수 (int)

- **`replace`**: 문자 대체 (r)
  - `selected_character`: 대체할 문자 (string)

- **`join_lines`**: 줄 합치기 (J)
  - `repeat`: 반복 횟수 (int)

- **`indent`**: 들여쓰기 (>/< 또는 >>/<<)
  - `repeat`: 반복 횟수 (int)
  - `forward`: 들여쓰기/내어쓰기 (bool)

### 검색
- **`search`**: 검색 모드 시작 (/)

### 네비게이션
- **`jump_list_walk`**: 점프 리스트 탐색 (Ctrl+O/I)
  - `repeat`: 반복 횟수 (int)
  - `forward`: 전진/후진 (bool)

### 코드 폴딩
- **`toggle_folding`**: 폴딩 토글 (za)
- **`fold_all`**: 모든 폴딩 (zM)
- **`unfold_all`**: 모든 폴딩 해제 (zR)

### 매크로
- **`record_macro`**: 매크로 녹화 시작 (q + 문자)
  - `selected_character`: 매크로 이름 (string)

- **`stop_record_macro`**: 매크로 녹화 중지 (q)

- **`play_macro`**: 매크로 재생 (@ + 문자)
  - `selected_character`: 매크로 이름 (string)
  - `repeat`: 반복 횟수 (int)

- **`repeat_last_edit`**: 마지막 편집 반복 (.)
  - `repeat`: 반복 횟수 (int)

### 북마크
- **`set_bookmark`**: 북마크 설정 (m + 문자)
  - `selected_character`: 북마크 이름 (string)

### 기타
- **`go_to_doc`**: 문서로 이동 (gd)

## 🔍 조건 함수 (CONDITION)

- **`is_recording`**: 매크로 녹화 중인지 확인
  - 반환값: boolean

## 📝 사용 예시

### 키 매핑에서 함수 사용
```json
{
    "keys": ["D"],
    "type": "OPERATOR_MOTION",
    "operator": "deletes",
    "motion": "move_to_end_of_line",
    "motion_args": { "inclusive": true }
}
```

### 조건부 실행
```json
{
    "keys": ["Q", "{char}"],
    "type": "ACTION",
    "action": "record_macro",
    "when_not": "is_recording"
}
```

### 복합 명령
```json
{
    "keys": ["5", "J"],
    "type": "MOTION",
    "motion": "move_by_lines",
    "motion_args": { 
        "forward": true, 
        "repeat": 5 
    }
}
```

## 💡 참고사항

- 모든 모션 함수는 `VimPosition` 객체를 반환합니다
- 오퍼레이터 함수는 선택된 텍스트에 대해 작동합니다
- 액션 함수는 즉시 실행되며 반환값이 없습니다
- `repeat` 인자는 대부분의 함수에서 지원됩니다
- `{char}` 키는 `selected_character` 인자로 전달됩니다

더 자세한 구현은 각 함수의 소스 코드를 참조하세요:
- **모션**: `src/vim/command/motions.cpp`
- **오퍼레이터**: `src/vim/command/operators.cpp`  
- **액션**: `src/vim/command/actlons.cpp`
- **조건**: `src/vim/command/condition.cpp`