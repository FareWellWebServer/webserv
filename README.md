# webserv
webserv
code style

- reference와 포인터 모두 자료형에 붙혀서 작성.
    ⭕️ : int& foo;
    ❌ : int & bar; or int &bar;
- return 괄호 없이
- void 반환 함수 일 때, return 사용 필수!
- tab 간격은 4칸.
- doxygen (vscode extension) 을 사용하여 작성한다.
- 주석은 함수 내부 말고 함수 선언 위에 작성할 것.
- 클래스 생성 시 public, protected, private 순서로 작성
- define 말고 enum 사용하기
- 일반변수는 snake case 
- 멤버변수는 snake case. 하지만 끝에 언더바 붙힘. (Ex. int this_is_example_)
- 모든 함수는 Pascal case. (Ex. void Example_Fun)
int num; // 일반변수 snake case

class example {

  int test_; // 멤버변수 snake case + _

  void Example_Function(); // 함수는 Pascal case

}


Commit Conventions
    commit의 기준
        commit은 아래 커밋 타입에 맞게 commit들을 분리한다.
    commit의 타입
        FEAT: 기능을 추가 또는 수정
        ENV: 개발 환경을 추가 또는 수정 (eslint 변경, dockerfile 변경 등)
        FIX: 버그를 해결
        DOCS: 문서를 수정 (README.md 변경, swagger)
        STYLE: 코드 스타일 변경 (prettier, npm run lint 등)
        REFACT: 코드를 리팩토링, 기능은 같고 코드가 변경
        TEST: 테스트 코드를 추가 또는 수정
        MERGE: 풀 리퀘스트 머지할 경우
        CHORE: 단순오타
        WIP: working in process 아직 작업중인 내용
    commit 예시
        ex)
            FIX: 모델 validation 오류 수정
            - Book title 제목 default 값 추가
            - User intra 최소 길이 0으로 변경
        ex)
            FEAT: 로그인 기능 추가
            - auth/ api 추가
        ex)
            TEST: bookController 테스트 코드 추가 
            - 책 제목에 대한 유효성 테스트 추가