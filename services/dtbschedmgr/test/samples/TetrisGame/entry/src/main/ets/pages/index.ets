import router from '@ohos.router';
import { TetrisViewModel, State, GameObject } from '../Model/TetrisViewModel'

if (!AppStorage.Has('state')) {
  AppStorage.Set('state', new State())
}
if (!AppStorage.Has('game')) {
  AppStorage.Set('game', new TetrisViewModel(new GameObject))
}
/**
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

@Entry
@Component
struct Index {
  @StorageLink('state') state: State = new State()
  @StorageLink('game') game: TetrisViewModel = new TetrisViewModel(new GameObject)

  build() {
    Row() {
      Column() {
        Blank().height('10%')
        Text('TETRIS')
          .textAlign(TextAlign.Center)
          .fontWeight(FontWeight.Bold)
          .fontColor(Color.Gray)
          .fontSize('80fp')
        Blank().height('5%')

        if (this.state.isStarted) {
          Button() {
            Text('    Continue    ')
              .fontSize('30fp')
              .fontWeight(FontWeight.Bold)
          }
          .backgroundColor(Color.Grey)
          .onClick(() => {
            router.push({ url: 'pages/GameTetrisView' })
            this.game.pauseBack()
          })
        }
        Blank().height('2%')
        Button() {
          Text('    NEW GAME    ')
            .fontSize('30fp')
            .fontWeight(FontWeight.Bold)
        }
        .backgroundColor(Color.Grey)
        .onClick(() => {
          router.push({ url: 'pages/GameTetrisView' })
          if (this.state.isStarted) {
            this.game.onEndGame()
          }
          this.game.newGame()
        })

        Blank().height('5%')

        Text('History')
          .textAlign(TextAlign.Center)
          .fontWeight(FontWeight.Bold)
          .fontSize('25fp')
        Blank().height('1%')
        List({ space: 3, initialIndex: 0 }) {
          ForEach(this.game.gameObject.history, (item, index) => {
            ListItem() {
              Row() {
                Text('NO. ' + (index + 1))
                  .width('50%')
                  .height('4%')
                  .fontSize('20fp')
                  .textAlign(TextAlign.Center)
                  .backgroundColor('')
                Text('' + item + (index == this.game.gameObject.index ? '(Now)' : ''))
                  .width('50%')
                  .height('4%')
                  .fontSize('20fp')
                  .textAlign(TextAlign.Center)
                  .backgroundColor('')
              }
            }
          })
        }
        .align(Alignment.Center)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('rgb(159,182,171)')
  }
}