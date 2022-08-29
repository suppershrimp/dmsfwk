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

import { TetrisViewModel, GameObject } from '../Model/TetrisViewModel'
import { BlockView } from './BlockView'
import { Figure } from '../Model/Figure'
import { BlockState } from '../Model/BlockState'

@Component
export struct NextFigureBarView {
  @StorageLink('game') game: TetrisViewModel = new TetrisViewModel(new GameObject)
  areaHeight: number[] = [3, 2, 1, 0]
  areaWeight: number[] = [0, 1, 2, 3]

  build() {
    Column() {
      Text('Next')
        .fontSize('20fp')
      Blank().height('10%')
      Stack() {
        Rect()
          .fill(Color.Gray)
          .opacity(0.4)
          .width('85%')
          .height('85%')
        Column() {
          ForEach(this.areaHeight, (i) => {
            Row() {
              ForEach(this.areaWeight, (j) => {
                Stack() {
                  if (this.game.gameObject.modelObject.nextFigureObject.container[i][j] == '*') {
                    BlockView({
                      opacity: 1
                    })
                  }
                  else {
                    BlockView({
                      opacity: 0
                    })
                  }
                }
                .height('25%')
                .width('25%')
              })
            }
          })
        }
        .width('85%')
        .height('85%')
      }
      .width('100%')
      .height('70%')
    }
    .width('70%')
    .height('25%')
  }
}