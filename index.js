const express = require("express");

const app = express();

app.listen(8081, function () {
  console.log("Conectado com sucesso!");
});

const Sequelize = require("sequelize");

// const sequelize = new Sequelize("esp32", "root", "",{

//     host: "localhost",

//     dialect: "mysql"

// })

const sequelize = new Sequelize(
  "postgres://bancoredis_user:4Sdso3dbzNm0Cre2BEyzYk6Bs8MXfDzD@dpg-ck1qi3vhdsdc73efvscg-a.oregon-postgres.render.com/bancoredis?ssl=true"
);

sequelize
  .authenticate()
  .then(function () {
    console.log("Conectado com sucesso!");
  })
  .catch(function (erro) {
    console.log("Falha ao conectar: " + erro);
  });

const Sensor = sequelize.define("sensors", {
  temperatura: {
    type: Sequelize.INTEGER,
  },

  umidade: {
    type: Sequelize.INTEGER,
  },
});

Sensor.sync({ force: true });

app.set("view engine", "ejs");
app.set("views", "./views");

app.get("/", async function (req, res) {
  try {
    var sensorsDataGet = await Sensor.findAll({
      order: [["createdAt", "ASC"]],
    });

    var sensorsDataJson = sensorsDataGet.map((sensor) => sensor.toJSON());
    var sensorsData = JSON.stringify(sensorsDataJson);

    res.render("./index", { sensorsData });
  } catch (error) {
    console.error("Erro ao buscar dados do Sensor:", error);
    res.status(500).send("Erro ao buscar dados do Sensor");
  }
});

app.get("/cadastrar/:temperatura/:umidade", function (req, res) {
  Sensor.create({
    temperatura: req.params.temperatura,

    umidade: req.params.umidade,
  })
    .then(function () {
      console.log("Cadastrado com sucesso!");
    })
    .catch(function (erro) {
      console.log("Falha ao cadastrar os dados: " + erro);
    });

  res.redirect("/");
});

app.get("/selectall", async function (req, res) {
  res.send(await Sensor.findAll());
});
