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

import { TetrisViewModel, State, GameObject } from '../Model/TetrisViewModel'
import { BlockView } from './BlockView'
import { BlockState } from '../Model/BlockState'

@Component
export struct GameView {
  @StorageLink('state') state: State = new State
  @StorageLink('game') game: TetrisViewModel = new TetrisViewModel(new GameObject)
  row: number[] = [19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0]
  column: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

  build() {
    if (this.state.update) {
      Flex() {
        Column() {
          ForEach(this.row, (i) => {
            Row() {
              ForEach(this.column, (j) => {
                Stack() {
                  BlockView({
                    opacity: this.getBlockOpacity(j, i)
                  })
                }
                .width('10%')
                .height('5%')
              })
            }
          })
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('rgb(159,182,171)')
    }
  }

  getBlockOpacity(x: number, y: number): number {
    if (this.game.gameObject.modelObject.fieldObject.container[y][x] == BlockState.fill) {
      return 1
    }
    else if (this.game.gameObject.modelObject.screen[y][x] == BlockState.fill) {
      return 1
    }
    else if (this.game.gameObject.modelObject.screen[y][x] == BlockState.shadow) {
      return 0.3
    }
    else {
      return 0.1
    }
  }
}