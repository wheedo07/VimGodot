# VimGodot 설정 가이드

VimGodot은 JSON 파일을 통해 키 매핑과 동작을 완전히 커스터마이징할 수 있습니다.

## 설정 파일 위치

- **키 매핑**: `res://addons/VimGodot/conf/key_maping.json`
- **화이트리스트**: `res://addons/VimGodot/conf/keys_white_list.json`

## 키 매핑 구조 (`key_maping.json`)

### 기본 구조
```json
{
    "keys": ["키조합"],
    "type": "명령타입",
    "context": "실행컨텍스트(선택사항)",
    "when": "실행조건(선택사항)",
    "when_not": "제외조건(선택사항)"
}
```

### 명령 타입별 설정

#### 1. MOTION (커서 이동)
```json
{
    "keys": ["H"],
    "type": "MOTION", 
    "motion": "move_by_characters",
    "motion_args": { 
        "forward": false, 
        "one_line": true 
    }
}
```

**주요 모션들:**
- `move_by_characters`: 문자 단위 이동
- `move_by_lines`: 줄 단위 이동  
- `move_by_words`: 단어 단위 이동
- `move_to_end_of_line`: 줄 끝으로 이동
- `move_to_start_of_line`: 줄 시작으로 이동
- `find_word_under_caret`: 커서 아래 단어 검색

#### 2. OPERATOR (텍스트 조작)
```json
{
    "keys": ["D"],
    "type": "OPERATOR",
    "operator": "deletes"
}
```

**주요 오퍼레이터들:**
- `deletes`: 삭제
- `yank`: 복사  
- `change`: 변경
- `change_case`: 대소문자 변경
- `toggle_case`: 대소문자 토글

#### 3. OPERATOR_MOTION (오퍼레이터 + 모션 조합)
```json
{
    "keys": ["Shift+D"],
    "type": "OPERATOR_MOTION",
    "operator": "deletes",
    "motion": "move_to_end_of_line",
    "motion_args": { "inclusive": true }
}
```

#### 4. ACTION (액션 실행)
```json
{
    "keys": ["I"],
    "type": "ACTION",
    "action": "enter_insert_mode",
    "action_args": { "insert_at": "inplace" }
}
```

**주요 액션들:**
- `enter_insert_mode`: Insert 모드 진입
- `enter_visual_mode`: Visual 모드 진입
- `paste`: 붙여넣기
- `undo`/`redo`: 실행취소/재실행
- `search`: 검색
- `record_macro`/`play_macro`: 매크로 녹화/재생

## 키 표기법

### 특수 키
- `Shift+키`: Shift 조합 (예: `Shift+G`)
- `Ctrl+키`: Ctrl 조합 (예: `Ctrl+F`) 
- `Alt+키`: Alt 조합
- `{char}`: 임의 문자 입력 (예: `["F", "{char}"]`)

### 기호 키
- `Shift+4`: `$`
- `Shift+6`: `^`
- `Shift+8`: `*`
- `Shift+9`/`Shift+0`: `()`
- `BracketLeft`/`BracketRight`: `[]`
- `Shift+BracketLeft`/`Shift+BracketRight`: `{}`

## 컨텍스트 설정

명령이 실행될 모드를 제한할 수 있습니다:

```json
{
    "keys": ["I"],
    "type": "ACTION", 
    "action": "enter_insert_mode",
    "context": "NORMAL"
}
```

**사용 가능한 컨텍스트:**
- `NORMAL`: Normal 모드에서만
- `VISUAL`: Visual 모드에서만
- 없으면 모든 모드에서 실행

## 조건부 실행

### when 조건
```json
{
    "keys": ["Q", "{char}"],
    "type": "ACTION",
    "action": "record_macro",
    "when_not": "is_recording"
}
```

**사용 가능한 조건:**
- `is_recording`: 매크로 녹화 중일 때

## 텍스트 객체 설정

```json
{
    "keys": ["I", "W"],
    "type": "MOTION",
    "motion": "text_object", 
    "motion_args": { 
        "inner": true, 
        "object": "w" 
    },
    "context": "NORMAL"
}
```

**텍스트 객체:**
- `w`: 단어 (`iw`, `aw`)
- `(`, `)`, `b`: 괄호 (`i(`, `a(`)
- `[`, `]`: 대괄호 (`i[`, `a[`)  
- `{`, `}`: 중괄호 (`i{`, `a{`)
- `"`, `'`: 인용부호 (`i"`, `a'`)

## 화이트리스트 설정 (`keys_white_list.json`)

VimGodot가 처리하지 않을 키를 지정:

```json
[
    "Ctrl+S",
    "Ctrl+Z", 
    "F5",
    "F11"
]
```

## 커스텀 매핑 예시

### 1. jj로 ESC 구현
```json
{
    "keys": ["J", "J"],
    "type": "ACTION", 
    "action": "enter_normal_mode"
}
```

### 2. 빠른 줄 이동 (5j, 5k)
```json
{
    "keys": ["5", "J"],
    "type": "MOTION",
    "motion": "move_by_lines",
    "motion_args": { "forward": true, "repeat": 5 }
}
```

### 3. 커스텀 검색
```json
{
    "keys": ["Shift+Slash"],
    "type": "ACTION",
    "action": "search_backward"
}
```

### 4. 폴딩 단축키
```json
{
    "keys": ["Space", "Space"],
    "type": "ACTION", 
    "action": "toggle_folding"
}
```

## 주의사항

1. **JSON 문법**: 올바른 JSON 형식을 유지해야 합니다
2. **키 충돌**: 기존 Godot 단축키와 충돌하지 않도록 주의
3. **백업**: 설정 변경 전 원본 파일을 백업하세요
4. **재시작**: 설정 변경 후 플러그인 재시작이 필요할 수 있습니다

## 디버깅

설정이 작동하지 않을 때 확인사항:
1. JSON 문법 오류 확인
2. 키 표기법이 정확한지 확인  
3. 필수 인자(`motion_args`, `action_args`)가 있는지 확인
4. 컨텍스트 설정이 올바른지 확인

더 자세한 정보는 `src/vim/command/` 폴더의 소스 코드를 참조하세요.