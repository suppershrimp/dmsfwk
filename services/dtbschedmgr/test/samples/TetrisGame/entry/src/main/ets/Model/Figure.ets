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

import { GameObject } from './TetrisViewModel';
import { GameField, FieldObject } from './GameField';
import { FigureTemplates } from './FigureTemplates';
import { BlockState } from './BlockState';

export class FigureObject {
  fieldRows: number
  fieldColumns: number
  figureType: FigureTemplates = new FigureTemplates
  typeNum: number = 1
  position: number[] = [5, 17]
  width: number = 4
  height: number = 4
  container: string[][] = []
  coord: number[][] = [[0, 0], [0, 0], [0, 0], [0, 0]]
  downCoord: number[][] = [[0, 0], [0, 0], [0, 0], [0, 0]]
  coordCnt: number = 0
  fieldObject: FieldObject = new FieldObject
  turn: number = 0
}

export class Figure {
  gameObject: GameObject
  gameField: GameField

  constructor(gameObject: GameObject, template?: number, position?: number[], field?: GameField) {
    this.gameObject = gameObject
    this.gameObject.modelObject.figureObject.fieldRows = this.gameObject.modelObject.rows
    this.gameObject.modelObject.figureObject.fieldColumns = this.gameObject.modelObject.columns
    if (template) {
      this.gameObject.modelObject.figureObject.typeNum = template
      this.containerInit(this.gameObject.modelObject.figureObject.figureType.figureType[this.gameObject.modelObject.figureObject.typeNum-1])
    }
    if (position) {
      this.gameObject.modelObject.figureObject.position = position
    }
    if (field) {
      this.gameField = field
      this.gameObject.modelObject.figureObject.fieldObject = this.gameField.gameObject.modelObject.fieldObject
    }
  }

  containerInit(template: string) {
    this.gameObject.modelObject.figureObject.container = []
    var j = 0
    var tmp: string[] = []
    for (var c of template) {
      tmp.push(c)
      j += 1
      if (j == this.gameObject.modelObject.figureObject.width) {
        this.gameObject.modelObject.figureObject.container.push(tmp)
        tmp = []
        j = 0
      }
    }
  }

  calcCoord() {
    var xx = 0, yy = 0
    this.gameObject.modelObject.figureObject.coordCnt = 0
    for (var i = 0;i < this.gameObject.modelObject.figureObject.width; i++) {
      for (var j = 0;j < this.gameObject.modelObject.figureObject.height; j++) {
        if (this.gameObject.modelObject.figureObject.container[j][i] == '*') {
          if (this.gameObject.modelObject.figureObject.turn == 1) {
            xx = this.gameObject.modelObject.figureObject.position[0] + (this.gameObject.modelObject.figureObject.height - j - 1)
            yy = this.gameObject.modelObject.figureObject.position[1] + i
          }
          else if (this.gameObject.modelObject.figureObject.turn == 2) {
            xx = this.gameObject.modelObject.figureObject.position[0] + (this.gameObject.modelObject.figureObject.width - i - 1)
            yy = this.gameObject.modelObject.figureObject.position[1] + (this.gameObject.modelObject.figureObject.height - j - 1)
          }
          else if (this.gameObject.modelObject.figureObject.turn == 3) {
            xx = this.gameObject.modelObject.figureObject.position[0] + j
            yy = this.gameObject.modelObject.figureObject.position[1] + (this.gameObject.modelObject.figureObject.height - i - 1) + (this.gameObject.modelObject.figureObject.width - this.gameObject.modelObject.figureObject.height)
          }
          else {
            xx = this.gameObject.modelObject.figureObject.position[0] + i
            yy = this.gameObject.modelObject.figureObject.position[1] + j
          }
          this.gameObject.modelObject.figureObject.coord[this.gameObject.modelObject.figureObject.coordCnt][0] = xx
          this.gameObject.modelObject.figureObject.coord[this.gameObject.modelObject.figureObject.coordCnt][1] = yy
          this.gameObject.modelObject.figureObject.coordCnt += 1
        }
      }
    }
    this.calcDownCoord()
  }

  calcDownCoord() {
    var dy = 0
    var isCollision = false
    while (!isCollision) {
      dy += 1
      for (var i = 0;i < this.gameObject.modelObject.figureObject.coordCnt; i++) {
        let y = this.gameObject.modelObject.figureObject.coord[i][1] - dy
        let x = this.gameObject.modelObject.figureObject.coord[i][0] < 0 ? 0 : (this.gameObject.modelObject.figureObject.coord[i][0] >= this.gameObject.modelObject.figureObject.fieldObject.columns ? this.gameObject.modelObject.figureObject.fieldObject.columns - 1 : this.gameObject.modelObject.figureObject.coord[i][0])
        if (y < 0 || this.gameObject.modelObject.figureObject.fieldObject.container[y][x] == BlockState.fill) {
          isCollision = true
        }
        this.gameObject.modelObject.figureObject.downCoord[i][0] = this.gameObject.modelObject.figureObject.coord[i][0]
        this.gameObject.modelObject.figureObject.downCoord[i][1] = y + 1
      }
    }
  }

  checkCollision(): CollisionType{
    this.calcCoord()
    for (var i = 0;i < this.gameObject.modelObject.figureObject.coordCnt; i++) {
      if (this.gameObject.modelObject.figureObject.coord[i][0] < 0 || this.gameObject.modelObject.figureObject.coord[i][0] >= this.gameObject.modelObject.figureObject.fieldObject.columns) {
        return CollisionType.side
      }
      if (this.gameObject.modelObject.figureObject.coord[i][1] >= this.gameObject.modelObject.figureObject.fieldObject.rows) {
        return CollisionType.up
      }
      if (this.gameObject.modelObject.figureObject.coord[i][1] < 0 || this.gameObject.modelObject.figureObject.fieldObject.container[this.gameObject.modelObject.figureObject.coord[i][1]][this.gameObject.modelObject.figureObject.coord[i][0]] == BlockState.fill) {
        return CollisionType.down
      }
    }
    return null
  }

  rotate() {
    let oldturn = this.gameObject.modelObject.figureObject.turn
    this.gameObject.modelObject.figureObject.turn = this.gameObject.modelObject.figureObject.turn + 1 > 3 ? 0 : this.gameObject.modelObject.figureObject.turn + 1
    if (this.checkCollision() == null) {
      this.calcCoord()
      return
    }
    else if (this.checkCollision() == CollisionType.side) {
      let xx = this.gameObject.modelObject.figureObject.position[0]
      let k: number = (this.gameObject.modelObject.figureObject.position[0] > (this.gameObject.modelObject.figureObject.fieldObject.columns / 2) ? -1 : +1)
      for (var i = 1;i < 3; i++) {
        this.gameObject.modelObject.figureObject.position[0] += k
        if (this.checkCollision() == null) {
          return
        }
      }
      this.gameObject.modelObject.figureObject.position[0] = xx
    }
    this.gameObject.modelObject.figureObject.turn = oldturn
    this.calcCoord()
  }

  move(dx: number, dy: number): boolean{
    let oldPositionX = this.gameObject.modelObject.figureObject.position[0]
    let oldPositionY = this.gameObject.modelObject.figureObject.position[1]
    this.gameObject.modelObject.figureObject.position[0] += dx
    this.gameObject.modelObject.figureObject.position[1] += dy
    var isCollision = this.checkCollision()
    if (isCollision != null) {
      this.gameObject.modelObject.figureObject.position[0] = oldPositionX
      this.gameObject.modelObject.figureObject.position[1] = oldPositionY
      this.calcCoord()
      if (isCollision == CollisionType.down) {
        return false
      }
    }
    return true
  }

  moveLeft(): boolean {
    return this.move(-1, 0)
  }

  moveRight(): boolean {
    return this.move(1, 0)
  }

  moveDown(): boolean {
    return this.move(0, -1)
  }

  fastMoveDown() {
    while (this.moveDown()) {
    }
  }

  putScreen(screen: BlockState[][]) {
    this.calcCoord()
    for (var i = 0;i < this.gameObject.modelObject.figureObject.coordCnt; i++) {
      if (this.gameObject.modelObject.figureObject.coord[i][1] >= 20) {
        continue
      }
      screen[this.gameObject.modelObject.figureObject.downCoord[i][1]][this.gameObject.modelObject.figureObject.downCoord[i][0]] = BlockState.shadow
    }
    for (var i = 0;i < this.gameObject.modelObject.figureObject.coordCnt; i++) {
      if (this.gameObject.modelObject.figureObject.coord[i][1] >= 20) {
        continue
      }
      screen[this.gameObject.modelObject.figureObject.coord[i][1]][this.gameObject.modelObject.figureObject.coord[i][0]] = BlockState.fill
    }
  }

  putField() {
    this.calcCoord()
    for (var i = 0;i < this.gameObject.modelObject.figureObject.coordCnt; i++) {
      this.gameObject.modelObject.fieldObject.container[this.gameObject.modelObject.figureObject.coord[i][1]][this.gameObject.modelObject.figureObject.coord[i][0]] = BlockState.fill
    }
  }
}

export enum CollisionType {
  side,
  down,
  up
}