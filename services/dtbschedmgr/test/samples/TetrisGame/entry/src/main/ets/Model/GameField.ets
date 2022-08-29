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

import { GameObject } from './TetrisViewModel'
import { BlockState } from './BlockState'
import { GameState } from './GameState'

export class FieldObject {
  container: BlockState[][] = []
  rows: number
  columns: number
}

export class GameField {
  gameObject: GameObject

  constructor(gameObject: GameObject) {
    this.gameObject = gameObject
    this.gameObject.modelObject.fieldObject.rows = this.gameObject.modelObject.rows
    this.gameObject.modelObject.fieldObject.columns = this.gameObject.modelObject.columns
    for (var i = 0; i < this.gameObject.modelObject.fieldObject.rows; i++) {
      var con: BlockState[] = []
      for (var j = 0;j < this.gameObject.modelObject.fieldObject.columns; j++) {
        con.push(BlockState.empty)
      }
      this.gameObject.modelObject.fieldObject.container.push(con)
    }
  }

  clear() {
    for (var i = 0;i < this.gameObject.modelObject.fieldObject.columns; i++) {
      for (var j = 0;j < this.gameObject.modelObject.fieldObject.rows; j++) {
        this.gameObject.modelObject.fieldObject.container[j][i] = BlockState.empty
      }
    }
  }

  put(screen: BlockState[][]) {
    for (var i = 0;i < this.gameObject.modelObject.fieldObject.columns; i++) {
      for (var j = 0;j < this.gameObject.modelObject.fieldObject.rows; j++) {
        screen[j][i] = this.gameObject.modelObject.fieldObject.container[j][i]
      }
    }
  }

  burning(): GameState{
    for (var j = 0;j < this.gameObject.modelObject.fieldObject.rows - 1; j++) {
      var fillLine = true
      for (var i = 0;i < this.gameObject.modelObject.fieldObject.columns; i++) {
        if (this.gameObject.modelObject.fieldObject.container[j][i] != BlockState.fill) {
          fillLine = false
        }
      }
      if (fillLine) {
        for (var y = j;y < this.gameObject.modelObject.fieldObject.rows - 1; y++) {
          for (var i = 0;i < this.gameObject.modelObject.fieldObject.columns; i++) {
            this.gameObject.modelObject.fieldObject.container[y][i] = this.gameObject.modelObject.fieldObject.container[y+1][i]
          }
        }
        return GameState.lineBurning
      }
    }
    return null
  }
}